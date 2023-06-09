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
