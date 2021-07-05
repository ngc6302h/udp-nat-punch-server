// udp_nat_puch_server.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#pragma warning(disable : 4996)

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

int main()
{
	WSADATA wsadata;
	int error = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (error != 0)
	{
		printf("Could not initialize WinSock. Error: %d\n", error);
		return 1;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		printf("Unsupported WinSock version.\n");
		WSACleanup();
		return 1;
	}

    SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);

	sockaddr_in address;
	address.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // *(ULONG*)localhost_info->h_addr_list[0];
	printf("Bound to: %s\n", inet_ntoa(address.sin_addr));

	address.sin_family = AF_INET;
	address.sin_port = htons(10000);
	bind(sock, (const sockaddr*)&address, sizeof(address));

	//main loop

	WSABUF buffer;
	buffer.buf = new CHAR[sizeof(in_addr) + sizeof(USHORT)];
	buffer.len = sizeof(in_addr) + sizeof(USHORT);
	DWORD bytesTransferred;
	DWORD flags = 0;
	sockaddr_in addrInfo1, addrInfo2;
	INT addr_info_size = sizeof(sockaddr_in);

	while (1)
	{
		int error = WSARecvFrom(sock, &buffer, 1, &bytesTransferred, &flags, (sockaddr*)&addrInfo1, &addr_info_size, NULL, NULL);
		if (error == SOCKET_ERROR)
		{
			std::cout << "WSARecvFrom failed with error: " << WSAGetLastError() << '\n';
		}
		std::cout << "Received packet from " << inet_ntoa(addrInfo1.sin_addr) << ':' << ntohs(addrInfo1.sin_port) << '\n';
		ZeroMemory(buffer.buf, 6);
		error = WSASendTo(sock, &buffer, 1, &bytesTransferred, 0, (const sockaddr*)&addrInfo1, addr_info_size, NULL, NULL);
		if (error == SOCKET_ERROR)
		{
			std::cout << "WSASendTo failed with error: " << WSAGetLastError() << '\n';
		}
		std::cout << "replied with zeros\n";
		do
		{
			error = WSARecvFrom(sock, &buffer, 1, &bytesTransferred, &flags, (sockaddr*)&addrInfo2, &addr_info_size, NULL, NULL);
			if (error == SOCKET_ERROR)
			{
				std::cout << "WSARecvFrom failed with error: " << WSAGetLastError() << '\n';
			}
			std::cout << "Received packet from " << inet_ntoa(addrInfo2.sin_addr) << ':' << ntohs(addrInfo2.sin_port) << '\n';
		} while (addrInfo1.sin_addr.S_un.S_addr == addrInfo2.sin_addr.S_un.S_addr);

		ULONG* ipAddrBufferPtr = (ULONG*) buffer.buf;
		*ipAddrBufferPtr = addrInfo2.sin_addr.S_un.S_addr;
		USHORT* portBufferPtr = (USHORT*)(buffer.buf + sizeof(ULONG));
		*portBufferPtr = addrInfo2.sin_port;
		error = WSASendTo(sock, &buffer, 1, &bytesTransferred, 0, (const sockaddr*)&addrInfo1, addr_info_size, NULL, NULL);
		if (error == SOCKET_ERROR)
		{
			std::cout << "WSASendTo failed with error: " << WSAGetLastError() << '\n';
		}
		std::cout << "replied to " << inet_ntoa(addrInfo1.sin_addr) << ':' << ntohs(addrInfo1.sin_port) << " with " << inet_ntoa(addrInfo2.sin_addr) << ':' << ntohs(addrInfo2.sin_port) << '\n';
	}
}