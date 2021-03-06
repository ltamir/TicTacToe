// server-skeleton 
// Listens on port 8888



#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>

#include <iostream>

#pragma warning(disable : 4996)
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define MAX_INPUT_CHAR 64

#define OK 0
#define NOK 1


int sendMessage(player_t * player, const char * message, char *inBuff);
int server_loop(player_t playerA, player_t playerB);
int readAck(player_t *pPlayer, char *inBuff);
void setupGame(player_t *player, player_t * player_a);

int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET server_socket;
	struct sockaddr_in server_address;
	int address_size;
	int result;	

	player_t playerA, playerB;
	player_t* pCurrPlayer = &playerA;

	playerA.state = PLAYER_STATE_INIT;
	playerB.state = PLAYER_STATE_INIT;
	playerA.sign = 0;
	playerB.sign = 0;

	// init Winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return NOK;
	}
	printf("Socket Initialised.\n");

	//Create a socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		return NOK;
	}

	u_short ServerPort = 8888;
	//Prepare the sockaddr_in structure
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY; // local host addr
	server_address.sin_port = htons(ServerPort); // Convert server port #  between host and network

	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
	}

	//Listen on socket 
	printf("\nGame server Ready...\n");
	listen(server_socket, 3); 

	address_size = sizeof(struct sockaddr_in);

	// connect to clients
	playerA.sd = accept(server_socket, (struct sockaddr *)&playerA.socket_client, &address_size);
	if (playerA.sd == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return NOK;
	}
	playerA.state = PLAYER_STATE_GREET;
	setupGame(&playerA, &playerA);

	playerB.sd = accept(server_socket, (struct sockaddr *)&playerB.socket_client, &address_size);
	if (playerA.sd == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return NOK;
	}
	playerB.state = PLAYER_STATE_GREET;
	setupGame(&playerB, &playerA);	

	result = server_loop(playerA, playerB);

	closesocket(server_socket);
	WSACleanup();

	return result;
}



int server_loop(player_t playerA, player_t playerB)
{
	const char *message;				// text for the client
	char *winnerMsg = NULL;				// pointer to winning message
	int msg_header = FLAG_EXIT;	// flag signaling client behaviour
	int readWriteFlag = 1;				// 1: write from socket, 2: read to socket
	int bytesRead;						// num of bytes read from socket
	int send_result;					// socket send result
	char buffIn[MAX_INPUT_CHAR] = "";		// input from client
	player_t *pCurrPlayer = &playerA;

	while (true)
	{
		// Game logic	
		/*
		A player struct holds a state field.
		Action is performed according to the state (semi state machine)
		*/

		//buffer must be nullified
		memset(buffIn, 0, MAX_INPUT_CHAR);	

		switch (pCurrPlayer->state)
		{

		case PLAYER_STATE_PRINT_BOARD:	//print board before selection
			setFlag(MSG_PRINT_BOARD, FLAG_PRINT_STRING);
			message = getMsg(MSG_PRINT_BOARD);
			send_result = sendMessage(pCurrPlayer, message, buffIn);

			setFlag(MSG_PRINT_AVAILABLE, FLAG_PRINT_STRING);
			message = getMsg(MSG_PRINT_AVAILABLE);
			send_result = sendMessage(pCurrPlayer, message, buffIn);

			pCurrPlayer->state = PLAYER_STATE_PROMPT_CELL;
			break;

		case PLAYER_STATE_PROMPT_CELL:
			setFlag(MSG_PROMPT_CELL, FLAG_PRINT_STRING);
			message = getMsg(MSG_PROMPT_CELL);
			send_result = sendMessage(pCurrPlayer, message, buffIn);

			pCurrPlayer->state = PLAYER_STATE_READ_CELL;
			break;
		case PLAYER_STATE_READ_CELL:
			setFlag(MSG_FLAG, FLAG_READ_CHAR);
			message = getMsg(MSG_FLAG);
			send_result = sendMessage(pCurrPlayer, message, buffIn);

			if (buffIn[0] == '0')
			{
				pCurrPlayer->state = PLAYER_STATE_EXIT;
			}
			pCurrPlayer->state = ApplyCell(buffIn[0], pCurrPlayer->sign);

			break;
		case PLAYER_STATE_STEP_ERR:
			setFlag(MSG_ERR_CELL, FLAG_PRINT_STRING);
			message = getMsg(MSG_ERR_CELL);
			send_result = sendMessage(pCurrPlayer, message, buffIn);
			pCurrPlayer->state = PLAYER_STATE_PROMPT_CELL;
			break;
		case PLAYER_STATE_NEXT:	//print board after selection & switch player
			setFlag(MSG_PRINT_BOARD, FLAG_PRINT_STRING);
			message = getMsg(MSG_PRINT_BOARD);
			send_result = sendMessage(pCurrPlayer, message, buffIn);

			setFlag(MSG_PRINT_AVAILABLE, FLAG_PRINT_STRING);
			message = getMsg(MSG_PRINT_AVAILABLE);
			send_result = sendMessage(pCurrPlayer, message, buffIn);

			setFlag(MSG_WAIT_FOR_YOUR_TURN, FLAG_PRINT_STRING);
			message = getMsg(MSG_WAIT_FOR_YOUR_TURN);
			send_result = sendMessage(pCurrPlayer, message, buffIn);
			
			pCurrPlayer->state = PLAYER_STATE_PRINT_BOARD;

			switchPlayer();
			break;
		case PLAYER_STATE_WINNER:
			setFlag(MSG_WINNER, FLAG_PRINT_WAIT);
			if (getLastPlayer() == NULL)
			{
				setLastPlayer(pCurrPlayer);
			}
			sprintf(buffIn, getMsg(MSG_WINNER), getLastPlayer()->name);
			
			message = buffIn;
			send_result = sendMessage(pCurrPlayer, message, buffIn);
			pCurrPlayer->state = PLAYER_STATE_EXIT;
			if (getLastPlayer() != pCurrPlayer) //if you'r the looser :( don't switch back to the winning player
				break;
			switchPlayer();
			pCurrPlayer = (getCurrentPlayer() == PLAYER_A ? &playerA : &playerB);
			pCurrPlayer->state = PLAYER_STATE_WINNER;
			break;
		case PLAYER_STATE_GAMEOVER:
			
			if (getLastPlayer() == NULL)
			{
				setLastPlayer(pCurrPlayer);
			}
			setFlag(MSG_GAME_OVER, FLAG_PRINT_WAIT);
			message = getMsg(MSG_GAME_OVER);
			send_result = sendMessage(pCurrPlayer, message, buffIn);
			pCurrPlayer->state = PLAYER_STATE_EXIT;
			if (getLastPlayer() != pCurrPlayer) //if other player was already set
				break;
			switchPlayer();
			pCurrPlayer = (getCurrentPlayer() == PLAYER_A ? &playerA : &playerB);
			pCurrPlayer->state = PLAYER_STATE_GAMEOVER;
			break;
		case PLAYER_STATE_EXIT:
			return OK;
			std::cout << "player " << pCurrPlayer->name << " reached PLAYER_STATE_EXIT" << std::endl;
			break;
		default:
			std::cout << "Invalid status for player" << pCurrPlayer->name << std::endl;
			break;
		}

		pCurrPlayer = (getCurrentPlayer() == PLAYER_A ? &playerA : &playerB);	// set current player
		
		if (send_result == NOK)
			return send_result;
	}

	return OK;
}


int sendMessage(player_t * player, const char * message, char *inBuff)
{
	int readMsg;
	int msg_length = strlen(message);
	if (send(player->sd, message, strlen(message), 0) < 0)
	{
		printf("Send failed");
		return NOK;
	}
	
	readMsg = readAck(player, inBuff);
	return OK;
}


int readAck(player_t *pPlayer, char *inBuff) {
	memset(inBuff, 0, MAX_INPUT_CHAR);
	
	int bytesRead = recv(pPlayer->sd, inBuff, 1024, 0);
	inBuff[bytesRead] = '\0';

	return 0;
}


void setupGame(player_t *pCurrPlayer, player_t * player_a)
{
	const char *buffOut;				// text for the client
	char buffIn[MAX_INPUT_CHAR];
	int send_result;
	int dataLength = -1;

	while (pCurrPlayer->state != PLAYER_STATE_PRINT_BOARD)
	{
		switch (pCurrPlayer->state)
		{
		case PLAYER_STATE_GREET:
			setFlag(MSG_GREETING, FLAG_PRINT_STRING);
			buffOut = getMsg(MSG_GREETING);
			send_result = sendMessage(pCurrPlayer, buffOut, buffIn);

			pCurrPlayer->state = PLAYER_STATE_PROMPT_NAME;
			//switchPlayer();
			break;
		case PLAYER_STATE_PROMPT_NAME:
			setFlag(MSG_PROMPT_NAME, FLAG_PRINT_STRING);
			buffOut = getMsg(MSG_PROMPT_NAME);
			send_result = sendMessage(pCurrPlayer, buffOut, buffIn);

			pCurrPlayer->state = PLAYER_STATE_READ_NAME;
			//switchPlayer();
			break;
		case PLAYER_STATE_READ_NAME:

			setFlag(MSG_FLAG, FLAG_READ_STRING);
			buffOut = getMsg(MSG_FLAG);
			printf("==> read name sending %d\n", buffOut[0]);
			send_result = sendMessage(pCurrPlayer, buffOut, buffIn);

			dataLength = strlen(buffIn);
			if (dataLength == 0)
			{
				pCurrPlayer->state = PLAYER_STATE_PROMPT_NAME;
				break;
			}
			strcpy(pCurrPlayer->name, buffIn);
			pCurrPlayer->name[dataLength] = '\0';
			pCurrPlayer->state = PLAYER_STATE_PROMPT_SIGN;
			break;
		case PLAYER_STATE_PROMPT_SIGN:
			if (player_a->sign == 0) {
				setFlag(MSG_PROMPT_SIGN, FLAG_PRINT_STRING);
				buffOut = getMsg(MSG_PROMPT_SIGN);
				send_result = sendMessage(pCurrPlayer, buffOut, buffIn);

				pCurrPlayer->state = PLAYER_STATE_READ_SIGN;
			}
			else if (player_a->sign == SIGN_X) {	// O left
				setFlag(MSG_PROMPT_SIGN_O, FLAG_PRINT_STRING);
				buffOut = getMsg(MSG_PROMPT_SIGN_O);
				send_result = sendMessage(pCurrPlayer, buffOut, buffIn);
				pCurrPlayer->sign = toupper(SIGN_O);
				pCurrPlayer->state = PLAYER_STATE_WAIT;
			}
			else {								// X left
				setFlag(MSG_PROMPT_SIGN_X, FLAG_PRINT_STRING);
				buffOut = getMsg(MSG_PROMPT_SIGN_X);
				send_result = sendMessage(pCurrPlayer, buffOut, buffIn);
				pCurrPlayer->sign = toupper(SIGN_X);
				pCurrPlayer->state = PLAYER_STATE_WAIT;
			}
			break;
		case PLAYER_STATE_READ_SIGN:
			setFlag(MSG_FLAG, FLAG_READ_CHAR);
			buffOut = getMsg(MSG_FLAG);
			printf("==> read sign sending %d\n", buffOut[0]);
			send_result = sendMessage(pCurrPlayer, buffOut, buffIn);

			if (toupper(buffIn[0]) != SIGN_X && toupper(buffIn[0]) != SIGN_O)
			{
				pCurrPlayer->state = PLAYER_STATE_PROMPT_SIGN;
				break;
			}

			pCurrPlayer->sign = toupper(buffIn[0]);

			pCurrPlayer->state = PLAYER_STATE_WAIT;
			break;
		default:
			std::cout << "setupGame: Invalid status for player " << pCurrPlayer->name << std::endl;
			break;
		case PLAYER_STATE_WAIT:
			setFlag(MSG_WAIT_PLAYER_2, FLAG_PRINT_STRING);
			buffOut = getMsg(MSG_WAIT_PLAYER_2);
			send_result = sendMessage(pCurrPlayer, buffOut, buffIn);		

			pCurrPlayer->state = PLAYER_STATE_PRINT_BOARD;
			break;
		}
	}

	
}