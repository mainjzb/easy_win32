#include "network.h"

#ifndef WIN32
#define WIN32
#endif
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include <flutter/method_result_functions.h>
#include <flutter/standard_method_codec.h>

#include <tuple>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")



namespace {
	using flutter::EncodableMap;
	using flutter::EncodableValue;
	using flutter::EncodableList;
	using flutter::MethodCall;
	using flutter::MethodResultFunctions;
}

std::tuple<std::string, DWORD> WideCharToMultiByteDemo(const std::wstring& wideStr)
{
	int multibyteLength = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if( multibyteLength == 0 )
	{
		return { "",GetLastError() };
	}

	std::string multibyteStr(multibyteLength, '\0');
	if( WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &multibyteStr[0], multibyteLength, nullptr, nullptr) == 0 )
	{
		return { "",GetLastError() };
	}

	return {multibyteStr, 0};
}


std::tuple<DWORD, DWORD> getDefaultInterfaceIndex()
{
	DWORD ifIndex = 0;
	sockaddr_in s;

	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.S_un.S_addr = inet_addr("8.8.8.8");

	DWORD err = GetBestInterfaceEx((sockaddr *)&s, &ifIndex);
	return {ifIndex, err};
}

std::tuple<EncodableMap, DWORD> getAdaptersAddresses(DWORD ifIndex)
{
    ULONG bufferSize = 0;
    PIP_ADAPTER_ADDRESSES pAddresses = nullptr;
    ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS;
	EncodableMap result;


	if( GetAdaptersAddresses(AF_INET, flags, nullptr, nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW )
	{
		pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(new BYTE[bufferSize]);

	}

    auto err = GetAdaptersAddresses(AF_INET, flags, nullptr, pAddresses, &bufferSize);
	if( err != NO_ERROR )
	{
		return { EncodableMap{}, err };
	}

   

	for( IP_ADAPTER_ADDRESSES* pAdapter = pAddresses; pAdapter; pAdapter = pAdapter->Next)
    {
        if (pAdapter->IfIndex == ifIndex)
		{
			// 获取网关
			{
				auto gateways = std::vector<EncodableValue>{};
				for (auto pGateway = pAdapter->FirstGatewayAddress; pGateway; pGateway = pGateway->Next)
				{
					sockaddr* pGatewayAddress = pGateway->Address.lpSockaddr;
					if (pGatewayAddress && pGatewayAddress->sa_family == AF_INET)
					{
						char gatewayBuffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(pGatewayAddress)->sin_addr, gatewayBuffer, sizeof(gatewayBuffer));
						gateways.push_back(std::string(gatewayBuffer));
					}
				}
				result[EncodableValue("gateway")] = EncodableList(gateways);
			}
			// 获取内网IP
			{
				auto ips = std::vector<EncodableValue>{};
				for( auto pGateway = pAdapter->FirstUnicastAddress; pGateway; pGateway = pGateway->Next )
				{
					sockaddr* pGatewayAddress = pGateway->Address.lpSockaddr;
					if( pGatewayAddress && pGatewayAddress->sa_family == AF_INET )
					{
						char buffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(pGatewayAddress)->sin_addr, buffer, sizeof(buffer));
						ips.push_back(std::string(buffer));
					}
				}
				result[EncodableValue("ip")] = EncodableList(ips);
			}
			// 获取DNS
			{
				auto dns = std::vector<EncodableValue>{};
				for( auto pGateway = pAdapter->FirstDnsServerAddress; pGateway; pGateway = pGateway->Next )
				{
					sockaddr* pGatewayAddress = pGateway->Address.lpSockaddr;
					if( pGatewayAddress && pGatewayAddress->sa_family == AF_INET )
					{
						char buffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(pGatewayAddress)->sin_addr, buffer, sizeof(buffer));
						dns.push_back(std::string(buffer));
					}
				}
				result[EncodableValue("dns")] = EncodableList(dns);
			}
			// 获取适配器名称
			{
				auto[friendlyName,error]=WideCharToMultiByteDemo(pAdapter->FriendlyName);
				if( error == 0 ) {
					result[EncodableValue("name")] = EncodableValue(friendlyName);
				}
			}
			// 获取适配器GUID
			{
				result[EncodableValue("guid")] = std::string(pAdapter->AdapterName);
			}
        }
    }

    // 释放内存
    if (pAddresses)
        delete[] reinterpret_cast<BYTE*>(pAddresses);


	return { result, 0 };
}

DWORD getDefaultInterfaceIP()
{
	DWORD err;
	DWORD ifIndex = 0;
	sockaddr_in s;
	DWORD ip = 0;

	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.S_un.S_addr = inet_addr("8.8.8.8");

	err = GetBestInterfaceEx((sockaddr *)&s, &ifIndex);
	if (err == NO_ERROR)
	{
		PIP_ADAPTER_ADDRESSES adapterAddresses = NULL;
		ULONG outBufferLength = 0;
		ULONG retVal = 0, i;

		for (i = 0; i < 5; i++)
		{
			retVal = GetAdaptersAddresses(
				AF_UNSPEC,
				0,
				NULL,
				adapterAddresses,
				&outBufferLength);

			if (retVal != ERROR_BUFFER_OVERFLOW)
			{
				break;
			}

			if (adapterAddresses != NULL)
			{
				free(adapterAddresses);
			}

			adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufferLength);
			if (adapterAddresses == NULL)
			{
				retVal = GetLastError();
				break;
			}
		}

		// Declare and initialize variables
		PIP_ADAPTER_INFO pAdapterInfo = NULL;
		DWORD dwRetVal = 0;
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

		// Call GetAdaptersInfo to get the size of the buffer needed
		dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

		// Check for errors
		if (dwRetVal != ERROR_BUFFER_OVERFLOW)
		{
			// cout << "GetAdaptersInfo failed with error " << dwRetVal << endl;
			return 1;
		}

		// Allocate memory for the buffer
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);

		// Call GetAdaptersInfo again to get the adapter information
		dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
		IP_ADAPTER_INFO *pAdapterInfoList = pAdapterInfo;
		while (pAdapterInfoList)
		{
			if (pAdapterInfoList->Index == ifIndex)
			{
				break;
			}
			pAdapterInfoList = pAdapterInfoList->Next;
		}

		if (pAdapterInfo != NULL)
		{
			free(pAdapterInfo);
		}

		if (retVal == NO_ERROR)
		{
			PIP_ADAPTER_ADDRESSES adapterList = adapterAddresses;
			while (adapterList)
			{
				if (adapterList->IfIndex == ifIndex)
				{
					// PIP_ADAPTER_UNICAST_ADDRESS pAddr = (PIP_ADAPTER_UNICAST_ADDRESS)adapterList->FirstUnicastAddress;
					PIP_ADAPTER_UNICAST_ADDRESS pAddr = (PIP_ADAPTER_UNICAST_ADDRESS)adapterList->FirstGatewayAddress;
					while (pAddr)
					{
						if (pAddr->Address.lpSockaddr->sa_family == AF_INET)
						{
							ip = ((sockaddr_in *)pAddr->Address.lpSockaddr)->sin_addr.S_un.S_addr;
							break;
						}
						pAddr = pAddr->Next;
					}
				}

				if (ip)
					break;

				adapterList = adapterList->Next;
			}
		}

		if (adapterAddresses != NULL)
		{
			free(adapterAddresses);
		}
	}

	return ip;
}