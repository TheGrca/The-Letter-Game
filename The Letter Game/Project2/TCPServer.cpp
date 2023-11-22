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

#define SERVER_PORT 27016
#define BUFFER_SIZE 256

// TCP server that use blocking sockets
int main()
{
	SOCKET listenSocket = INVALID_SOCKET; // Socket used for listening for new clients 
	SOCKET acceptedSocket1 = INVALID_SOCKET; // Socket used for communication with client 1
	SOCKET acceptedSocket2 = INVALID_SOCKET; // Socket used for communication with client 2

	int iResult;
	bool first, second;
	bool quit1, quit2;
	int words1, words2;
	int length1, length2;

	char dataBuffer1[BUFFER_SIZE];
	char dataBuffer2[BUFFER_SIZE];


	WSADATA wsaData;   // Initialize Winsock library
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}


	// Initialize serverAddress structure 
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;				// IPv4 address family
	serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
	serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port


	// Create a SOCKET for connecting to server
	listenSocket = socket(AF_INET,      // IPv4 address family
		SOCK_STREAM,  // Stream socket
		IPPROTO_TCP); // TCP protocol
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind TCP listening socket to port number and local address
	iResult = bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	iResult = listen(listenSocket, SOMAXCONN); 	// Set listenSocket in listening mode
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	do
	{
		// Struct for information about connected client
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(struct sockaddr_in);

		// Accept the first client
		acceptedSocket1 = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (acceptedSocket1 == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		printf("\nFirst client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		//Accept the second client
		acceptedSocket2 = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (acceptedSocket2 == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			closesocket(acceptedSocket1);
			WSACleanup();
			return 1;
		}
		printf("\nSecond client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		printf("Enter a letter on which play starts: ");
		char Letter;
		scanf_s(" %c", &Letter);
		getchar();

		sprintf_s(dataBuffer1, "The game start with the letter %c. Send your word!\n", Letter);
		sprintf_s(dataBuffer2, "The game start with the letter %c. Send your word!\n", Letter);

		words1 = words2 = 0;    //count number of correct words of first and second player
		length1 = length2 = 0;  //total length of correct words of first and second player
		quit1 = quit2 = false;  //indicates if player quits the game
		bool end = false;     // indicator of the game end

		do
		{
			// Send message to client
			iResult = send(acceptedSocket1, dataBuffer1, (int)strlen(dataBuffer1), 0);
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				shutdown(acceptedSocket1, SD_BOTH);
				shutdown(acceptedSocket2, SD_BOTH);
				closesocket(acceptedSocket1);
				closesocket(acceptedSocket2);
				break;
			}

			// Send message to client
			iResult = send(acceptedSocket2, dataBuffer2, (int)strlen(dataBuffer2), 0);
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				shutdown(acceptedSocket1, SD_BOTH);
				shutdown(acceptedSocket2, SD_BOTH);
				closesocket(acceptedSocket1);
				closesocket(acceptedSocket2);
				break;
			}

			//if the play between two players ends
			if (end)
				break;


			// Receive data from first client 
			iResult = recv(acceptedSocket1, dataBuffer1, BUFFER_SIZE, 0);
			if (iResult > 0)	// Check if message is successfully received
			{
				dataBuffer1[iResult] = '\0';

				// Log message
				printf("Client 1 sent: %s.\n", dataBuffer1);
				if (!strcmp(dataBuffer1, "End"))  //First player is quitting the game
				{
					quit1 = true;
					first = false;
				}
				else
				{
					//Check if player sent word on selected letter.
					if (toupper(dataBuffer1[0]) == toupper(Letter))
					{
						printf("Correct word! \n");
						first = true;
						words1++;
						length1 += strlen(dataBuffer1);
					}
					else
					{
						printf("Incorrect word! \n");
						first = false;
					}
				}

			}
			else if (iResult == 0)	// Check if shutdown command is received
			{
				printf("Connection with first client closed.\n");
				shutdown(acceptedSocket2, SD_BOTH);
				closesocket(acceptedSocket1);
				closesocket(acceptedSocket2);
				break;
			}
			else	// There was an error during recv function
			{
				printf("recv failed with error: %d\n", WSAGetLastError());
				shutdown(acceptedSocket1, SD_BOTH);
				shutdown(acceptedSocket2, SD_BOTH);
				closesocket(acceptedSocket1);
				closesocket(acceptedSocket2);
				break;
			}


			// Receive data from second client 
			iResult = recv(acceptedSocket2, dataBuffer2, BUFFER_SIZE, 0);

			if (iResult > 0)	// Check if message is successfully received
			{
				dataBuffer2[iResult] = '\0';
				// Log message 
				printf("Client 2 sent: %s.\n", dataBuffer2);

				if (!strcmp(dataBuffer2, "End"))    //second player is quitting
				{
					quit2 = true;
					second = false;

				}
				else
				{
					//Check if player sent word on selected letter
					if (toupper(dataBuffer2[0]) == toupper(Letter))
					{
						printf("Correct word! \n");
						second = true;
						words2++;
						length2 += strlen(dataBuffer2);
					}
					else
					{
						printf("Incorrect word! \n");
						second = false;
					}
				}

			}
			else if (iResult == 0)	// Check if shutdown command is received
			{
				printf("Connection with client closed.\n");
				shutdown(acceptedSocket1, SD_BOTH);
				closesocket(acceptedSocket1);
				closesocket(acceptedSocket2);
				break;
			}
			else	// There was an error during recv
			{
				printf("recv failed with error: %d\n", WSAGetLastError());
				shutdown(acceptedSocket1, SD_BOTH);
				shutdown(acceptedSocket2, SD_BOTH);
				closesocket(acceptedSocket1);
				closesocket(acceptedSocket2);
				break;
			}

			// the play continues if both clients have good words or both mistake
			if (first == second)
			{
				strcpy_s(dataBuffer1, "Please send next word on chosen letter.");
				strcpy_s(dataBuffer2, "Please send next word on chosen letter.");
			}
			//If one has made a mistake and the other has not (he is the winner)
			else
			{
				//first player wins
				if (first)
				{
					sprintf_s(dataBuffer1, "The first player sent %d correct words, the second player sent %d correct words. You have WON!\n", words1, words2);
					sprintf_s(dataBuffer2, "The first player sent %d correct words, the second player sent %d correct words. You have LOST!\n", words1, words2);
					end = true;
				}
				//second player wins
				if (second)
				{
					sprintf_s(dataBuffer1, "The first player sent %d correct words, the second player sent %d correct words. You have LOST!\n", words1, words2);
					sprintf_s(dataBuffer2, "The first player sent %d correct words, the second player sent %d correct words. You have WON!\n", words1, words2);
					end = true;
				}

			}
			// if both players quit at the same time
			if (quit1 && quit2)
			{
				if (length1 > length2)
				{
					sprintf_s(dataBuffer1, "The first player sent %d correct words, the second player sent %d correct words. The total length of your words is longer. You have WON!\n", words1, words2);
					sprintf_s(dataBuffer2, "The first player sent %d correct words, the second player sent %d correct words. The total length of your words is shorter. You have LOST!\n", words1, words2);
					end = true;
				}
				else
				{
					sprintf_s(dataBuffer1, "The first player sent %d correct words, the second player sent %d correct words. The total length of your words is shorter. You have LOST!\n", words1, words2);
					sprintf_s(dataBuffer2, "The first player sent %d correct words, the second player sent %d correct words. The total length of your words is longer. You have WON!\n", words1, words2);
					end = true;
				}
			}
		} while (true);


	} while (true);

	// Shutdown the connection since we're done
	iResult = shutdown(acceptedSocket1, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket1);
		closesocket(acceptedSocket2);
		WSACleanup();
		return 1;
	}

	// Shutdown the connection since we're done
	iResult = shutdown(acceptedSocket2, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket1);
		closesocket(acceptedSocket2);
		WSACleanup();
		return 1;
	}

	//Close listen and accepted sockets
	closesocket(listenSocket);
	closesocket(acceptedSocket1);
	closesocket(acceptedSocket2);

	WSACleanup();
	return 0;
}