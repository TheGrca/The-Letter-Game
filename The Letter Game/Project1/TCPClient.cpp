#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "127.0.0.1" //Set local IP Adress of server
#define SERVER_PORT 27016 //Set Port of Server
#define BUFFER_SIZE 256


int main()
{
	WSADATA wsaData; 	// Initialize Winsock library
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	SOCKET connectSocket = INVALID_SOCKET;
	int iResult; // Variable used to store function return value
	char dataBuffer[BUFFER_SIZE]; // Buffer we will use to store message


	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a socket
	if (connectSocket == INVALID_SOCKET)
	{
		printf("Socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;								// IPv4 protocol
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);	// ip address of server
	serverAddress.sin_port = htons(SERVER_PORT);					// server port

	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) // Connect to server
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	bool end = false;

	do {

		// Receive  message from server
		iResult = recv(connectSocket, dataBuffer, BUFFER_SIZE, 0);

		if (iResult > 0)	// Check if message is successfully received
		{
			dataBuffer[iResult] = '\0'; // Set last byte to '\0'
			printf("Server sent: %s.\n", dataBuffer);
			if (strstr(dataBuffer, "WON") || strstr(dataBuffer, "LOST"))
				end = true;
		}
		else if (iResult == 0)	// Check if shutdown command is received
		{
			printf("Connection with server closed.\n");
			closesocket(connectSocket);
			WSACleanup();
			return 0;
		}
		else	// There was an error during recv function
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		if (end)
			break;

		printf("Enter word on selected letter to send: ");
		gets_s(dataBuffer, BUFFER_SIZE);


		// Send message to server 
		iResult = send(connectSocket, dataBuffer, (int)strlen(dataBuffer), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
	} while (true);


	// Shutdown the connection 
	iResult = shutdown(connectSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("\nPress any key to exit: ");
	_getch();

	closesocket(connectSocket);
	WSACleanup();
	return 0;
}