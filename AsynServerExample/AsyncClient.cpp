// Async-client1.cpp : Socket client supporting async user input and socket comm using select()
// Polling user KBD input and sending key by key to server
// Polling socket with select() for Rx data from server and displays Rx data on screen

#include "stdafx.h"
#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>


#include <iostream>
#include <WS2tcpip.h>

#pragma warning(disable : 4996)
#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

int main1()
{
	WSADATA wsa; //  structure contains information about the Windows Sockets implementation.
	SOCKET s; // 

	struct sockaddr_in server;  // structure containing an internet address
//	const char *message;
//	char server_reply[3000];
	char user_in[1];
	char ip_addr[30];
	int recv_size;
	char ch, ch1;
	struct timeval tv;

	printf("Enter Server IP address: ");
	fgets(ip_addr, 20, stdin);
	printf("\n Server IP address= %s", ip_addr);

	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // 	Initialise Winsock
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}
	printf("Socket Initialised.\n");

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) // create a socket
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	printf("connecting socket to server....");
	server.sin_addr.s_addr = inet_addr(ip_addr); // inet_addr () converts string in IPv4 dotted decimal notation, to an integer  suitable for use as an Internet address.
	server.sin_family = AF_INET; // AF_INET == address family designates type of addresses socket can communicate with == IPv4 
	server.sin_port = htons(8888);  //convert server port from host to network BIG Endian format

									//Connect to remote server
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("connect error");
		return 1;
	}
	printf("Connected to server \n");

	// For Async Rx from server
	// Create the "fd_set" master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master); // at this point there are no sockets in the fd_set array

					  // Add our connected socket to the master fd_set array to be polled by select()					   
	FD_SET(s, &master);

	// Async user input
	bool repeat = true;
	while (repeat)
	{
		Sleep(200);
		if (kbhit() != 0) // check for user KBD input
		{
			ch = getch();
			switch (ch)
			{
			case 27:               // press ESC to exit
				repeat = false;
				return 0;
				break;
			default:               // any other but ESC  
				if (ch == '\r') {
					printf("\n");
				}
				else {
					printf("%c", ch);
				}
				ch1 = getch();  // Temp - check and fix need for 2nd read - Eyal 
				user_in[0] = ch;
				if (send(s, user_in, 1, 0) < 0)  // send user input to server
				{
					printf("Send failed");
					return 1;
				}
				if (ch == '\r') {      // if user pressed CR then also send NL to server
					user_in[0] = '\n';
					send(s, user_in, 1, 0);
				}
				// Done sending user input to server
				break;
			}
		}
		// check for data Rx from server
		fd_set copy = master;  // make a copy as select() destorys its fd_set array parameter

							   // Calling select() with Time Out that returns when server data Rx or timer expires
							   // time-out points to a TIMEVAL structure that specifies the max time select)( should wait before
							   // returning. When select returns, the contents of the TIMEVAL structure are not altered.
							   // If TIMEVAL is initialized to {0, 0}, select will return immediately - used to poll the
							   // state of the selected sockets.
		tv.tv_sec = 0; // TIMEVAL initialized to {0, 0}, select will return immediately
		tv.tv_usec = 0;
		int socketCount = select(0, &copy, nullptr, nullptr, &tv); // poll socket Rx  
		if (socketCount == -1) {
			printf("select err"); // error occurred in select()
		}
		// if (socketCount == 0) printf("Timeout occurred!  No data to read.\n");
		if (socketCount > 0) // Data Rx from server
		{
			char buf[4096];
			ZeroMemory(buf, 4096);
			// Receive message
			int bytesIn = recv(s, buf, 4096, 0);
			if (bytesIn <= 0)
			{
				// Drop the client
				printf("Rx error");
				closesocket(s);
				FD_CLR(s, &master);
				return 1;
			}
			printf("%s", buf);
		}
	}


	closesocket(s); //close socket 
	WSACleanup();

	return 0;
}


