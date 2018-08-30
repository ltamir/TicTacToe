#pragma once

#include<winsock2.h>
#include "TicTacToeProtocol.h"

#define BOARD_SIZE 9
#define PRINT_BOARD_SIZE 9 + 3 + 1
#define CELL_EMPTY 0x20	//0x3F
#define CELL_OCCUPIED 0x2D

#define BORDER_HORIZONTAL 0x2D
#define BORDER_VERTICAL	0x7C
#define CRLF	0xA

#define SIGN_X 0x58
#define SIGN_O 0x4F

/* PLAYER State*/
#define PLAYER_STATE_GREET 1
#define PLAYER_STATE_PROMPT_NAME 2
#define PLAYER_STATE_PROMPT_SIGN 3
#define PLAYER_STATE_PRINT_BOARD 4
#define PLAYER_STATE_PRINT_AVAILABLE 5
#define PLAYER_STATE_PROMPT_CELL 6
#define PLAYER_STATE_READ_CELL 7
#define PLAYER_STATE_NEXT 8
#define PLAYER_STATE_READ_NAME 9
#define PLAYER_STATE_READ_SIGN 10
#define PLAYER_STATE_STEP_ERR 11
#define PLAYER_STATE_GAMEOVER 12
#define PLAYER_STATE_EXIT 13
#define PLAYER_STATE_WAIT 14

#define PLAYER_STATE_INIT 0

#define PLAYER_A 1
#define PLAYER_B 2

typedef struct 
{
	char sign;
	char name[65] = {0};
	SOCKET sd;
	struct sockaddr_in socket_client;
	int state;
}player_t;



#define MSG_GREETING 0
#define MSG_PROMPT_NAME 1
#define MSG_PROMPT_SIGN 2
#define MSG_PROMPT_SIGN_X 3
#define MSG_PROMPT_SIGN_O 4
#define MSG_PROMPT_CELL 5
#define MSG_ERR_CELL 6
#define MSG_WAIT_PLAYER_2 7
#define MSG_ERR_WINNER 8
#define MSG_GAME_OVER 9
#define MSG_WAIT_FOR_YOUR_TURN 10
#define MSG_FLAG 11
#define MSG_PRINT_BOARD 12
#define MSG_PRINT_AVAILABLE 13

#define ONE 0x31
#define TWO 0x32
#define THREE 0x33
#define FOUR 0x34
#define FIVE 0x35
#define SIX 0x36
#define SEVEN 0x37
#define EIGHT 0x38
#define NINE 0x39


void switchPlayer();

int ApplyCell(char cell, char sign);

const char* getMsg(int msgId);

void setFlag(int msgId, char flag);

int getCurrentPlayer();

void setCurrentPlayer(player_t * player);
