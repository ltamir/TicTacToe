// Async_-server1.cpp 
// Using select() to enable multi client communications overcoming blocking function issues
// select() returns when:
// 1. New client sends connect() --> call accept() and add new socket to fd_set array for select() to monitor
// 2. Data Rx on an existing socket connection --> call recv() to get data and send out to all other clients

#include <iostream>
#include <WS2tcpip.h>
#include "stdafx.h"

#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include <stdlib.h>
#include <cstdlib>

#pragma warning(disable : 4996)
#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

// int main()
int main() // (int argc, char *argv[])
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	const char *message;
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		printf("Can't Initialize winsock! Quitting \n");
		return 1;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		printf("Can't create a socket! Quitting\n");
		return 1;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8888); //54000
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Create the "fd_set" master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master); // at this point there are no sockets in the fd_set array

					  // Add our first socket that we're interested in interacting with --> the listening socket!
					  // It's important that this socket is added for our server or else we won't 'hear' incoming connections 
	FD_SET(listening, &master);

	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = master;

		// See who's talking to us
		// Calling select() that returns when a relevant event occurs - new connection, client data Rx
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr); // block until event occurs

																	   // Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			// Is it an inbound communication?
			if (sock == listening)
			{
				// Accept a new connection
				printf("\naccept new connection...");
				SOCKET client = accept(listening, nullptr, nullptr);
				printf("new connection accepted \n");
				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				message = "Welcome from server\r\n";
				if (send(client, message, strlen(message), 0) < 0)
				{
					printf("Send failed");
					return 1;
				}
				printf("\n Sent welcome msg to client\n");
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				printf("%s", buf); //  num of bytes = %d", bytesIn);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					/* To be completed
					// Check to see if it's a command. \quit kills the server
					if (buf[0] == '\\')
					{
					// Is the command quit?
					string cmd = string(buf, bytesIn);
					if (cmd == "\\quit")
					{
					running = false;
					break;
					}

					// Unknown command
					continue;
					}
					*/

					// Send message to other clients, definiately NOT the listening socket

					for (u_int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							send(outSock, buf, strlen(buf), 0);
						}
					}
				}
			}
		}
	}

	// Cleanup winsock
	WSACleanup();
	//system("pause");  Eyal check
	return 0;
}

// shut down sequence
// Remove the listening socket from the master file descriptor set and close it
// to prevent anyone else trying to connect.
/* FD_CLR(listening, &master);  Eyal fix
closesocket(listening);

// Message to let users know what's happening.
string msg = "Server is shutting down. Goodbye\r\n";

while (master.fd_count > 0)
{
// Get the socket number
SOCKET sock = master.fd_array[0];

// Send the goodbye message
send(sock, msg.c_str(), msg.size() + 1, 0);

// Remove it from the master file list and close the socket
FD_CLR(sock, &master);
closesocket(sock);
}
*/



