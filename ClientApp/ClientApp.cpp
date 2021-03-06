// Socket client skeleton code 
// In this example - get server IP from user, server port is fixed to 8888
// Create your client side app on top of this



#include <string.h>
#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h> // / Winsock Library contains definitions to be used with the WinSock2 DLL and WinSock 2 applications.
#include "TicTacToeProtocol.h"

#pragma comment(lib,"ws2_32.lib") // The #pragma comment indicates to the linker that the Ws2_32.lib file is needed.
#pragma warning(disable : 4996) // Visual Studio Compiler Directive - turn warning off - e.g. with fopen()

#define EXIT_GAME 0

/*
#define FLAG_EXIT 1
#define FLAG_PRINT_WAIT 2
#define FLAG_PRINT_STRING 3
#define FLAG_READ_CHAR 4
#define FLAG_READ_STRING 5
*/


void print(char *serverMsg, int size);
int sendMsg(SOCKET socket, char *msg);
void printBuff(char *buff, int size);

int main(int argc, char *argv[])
{
	WSADATA wsa; //  structure contains information about the Windows Sockets implementation.
	SOCKET clientSocket; // 

	struct sockaddr_in server;  // structure containing an internet address
	const char *message;
	char user_char = -1;
	char user_string[65];
	
	char server_reply[100];
	char ip_addr[30] = "127.0.0.1";

#define DEFAULT_SERVER_IP "127.0.0.1"
	int recv_size;
	int msg_header = FLAG_EXIT;

	char header = 0;
	char *body = NULL;


	printf("Enter Server IP [Enter for 127.0.0.1]:");
	fgets(ip_addr, 20, stdin);
	if (strlen(ip_addr) == 1)	// just \n
	{
		strcpy(ip_addr, DEFAULT_SERVER_IP);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // 	Initialise Winsock
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) // create a socket
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("connecting socket to server....");
	server.sin_addr.s_addr = inet_addr(ip_addr); 
	server.sin_family = AF_INET; 
	server.sin_port = htons(8888);  //convert server port from host to network BIG Endian format


	// connection to server
	if (connect(clientSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("connection error");
		printf("error code : %d", WSAGetLastError());
		return 1;
	}
	printf("Connected to TicTacToe server \n");


	// loop with server
	while (user_char != EXIT_GAME)
	{
		memset(server_reply, 0, 100);
		memset(user_string, 0, 65);
		
		if ((recv_size = recv(clientSocket, server_reply, 99, 0)) == SOCKET_ERROR) // waiting for server reply
		{
			printf("recv failed");
			return 1;
		}

		//printBuff(server_reply, recv_size);	// debug

		header = server_reply[0];	// read header
		body = &server_reply[1];	// point to beginning of body


		switch (header) {
		case FLAG_EXIT:
			print(body, recv_size);
			user_string[0] = 1;
			getchar();
			user_char = EXIT_GAME;
			sendMsg(clientSocket, user_string);
			break;
		case FLAG_PRINT_WAIT:
			puts(body);
			user_string[0] = 1;
			puts("press any key to continue\n");
			sendMsg(clientSocket, user_string);
			getchar();
			break;
		case FLAG_PRINT_STRING:
			print(body, recv_size);
			user_string[0] = 1;
			sendMsg(clientSocket, user_string);
			break;
		case FLAG_READ_CHAR:
			fgets(user_string, 64, stdin);

			message = user_string;
			if (send(clientSocket, message, 1, 0) < 0)
			{
				printf("Send failed");
				return 1;
			}

			break;
		case FLAG_READ_STRING:
			fgets(user_string, 64, stdin);
			message = user_string;
			if (send(clientSocket, message, strlen(message), 0) < 0)
			{
				printf("Send failed");
				return 1;
			}
			break;
		}
		
	}


	printf("Thank you for TicTacTo-ing\n");
	getchar();
	closesocket(clientSocket); 
	WSACleanup();

	return 0;
}

void print(char *serverMsg, int size)
{
	serverMsg[size] = '\0';

	printf("\n%s", serverMsg);	//2nd char starts the text message from server

}
int sendMsg(SOCKET socket, char *buff)
{
	if (send(socket, buff, 1, 0) < 0)
	{
		printf("Send failed");
		return 1;
	}

	return 0;
}

void printBuff(char *buff, int size) 
{
	int pos;
	printf("\n read: ");
	for (pos = 0; pos < size; pos++)
	{
		if (buff[pos] >= 32 && buff[pos] <= 126)
			printf("%c", buff[pos]);
		else
			printf("%d", buff[pos]);
	}
	printf("\n");
}