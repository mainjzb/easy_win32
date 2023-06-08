#include "network.h"

#ifndef WIN32
#define WIN32
#endif
#include <WinSock2.h>
#include <ws2tcpip.h>

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include<tuple>

std::tuple<DWORD, DWORD> getDefaultIFIndex()
{
	DWORD ifIndex = 0;
	sockaddr_in s;

	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.S_un.S_addr = inet_addr("8.8.8.8");

	DWORD err = GetBestInterfaceEx((sockaddr*)&s, &ifIndex);
	return { ifIndex, err };
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

	err = GetBestInterfaceEx((sockaddr*)&s, &ifIndex);
	if( err == NO_ERROR )
	{
		PIP_ADAPTER_ADDRESSES adapterAddresses = NULL;
		ULONG outBufferLength = 0;
		ULONG retVal = 0, i;

		for( i = 0; i < 5; i++ )
		{
			retVal = GetAdaptersAddresses(
				AF_UNSPEC,
				0,
				NULL,
				adapterAddresses,
				&outBufferLength);

			if( retVal != ERROR_BUFFER_OVERFLOW )
			{
				break;
			}

			if( adapterAddresses != NULL )
			{
				free(adapterAddresses);
			}

			adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufferLength);
			if( adapterAddresses == NULL )
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
		if( dwRetVal != ERROR_BUFFER_OVERFLOW ) {
			//cout << "GetAdaptersInfo failed with error " << dwRetVal << endl;
			return 1;
		}

		// Allocate memory for the buffer
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);

		// Call GetAdaptersInfo again to get the adapter information
		dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
		IP_ADAPTER_INFO* pAdapterInfoList = pAdapterInfo;
		while( pAdapterInfoList ) {
			if( pAdapterInfoList->Index == ifIndex ) {
				break;
			}
			pAdapterInfoList = pAdapterInfoList->Next;
		}

		if( pAdapterInfo != NULL ) {
			free(pAdapterInfo);
		}



		if( retVal == NO_ERROR )
		{
			PIP_ADAPTER_ADDRESSES adapterList = adapterAddresses;
			while( adapterList )
			{
				if( adapterList->IfIndex == ifIndex )
				{
					//PIP_ADAPTER_UNICAST_ADDRESS pAddr = (PIP_ADAPTER_UNICAST_ADDRESS)adapterList->FirstUnicastAddress;
					PIP_ADAPTER_UNICAST_ADDRESS pAddr = (PIP_ADAPTER_UNICAST_ADDRESS)adapterList->FirstGatewayAddress;
					while( pAddr )
					{
						if( pAddr->Address.lpSockaddr->sa_family == AF_INET )
						{
							ip = ((sockaddr_in*)pAddr->Address.lpSockaddr)->sin_addr.S_un.S_addr;
							break;
						}
						pAddr = pAddr->Next;
					}
				}

				if( ip )
					break;

				adapterList = adapterList->Next;
			}
		}

		if( adapterAddresses != NULL )
		{
			free(adapterAddresses);
		}
	}

	return ip;
}