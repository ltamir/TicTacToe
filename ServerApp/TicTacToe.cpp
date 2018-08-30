#include "stdafx.h"
#include "TicTacToe.h"
#include "stdlib.h"

char msg_table[14][50] = {
	" Welcome to TicTacToe\n",								//0
	" Enter player name:",									//1
	" Select your sign: X or O:",							//2
	" Available sign is X",									//3
	" Available sign is O",									//4
	" Enter cell number:",									//5
	" Invalid cell. Only available cells are allowed.\n",	//6
	" Please wait for the other player...",					//7
	" Player %s is the winner!",							//8
	" Game Over.",											//9
	" Please wait for your next move",						//10
	" ",													//11
	" ",													//12
	" "														//13
};



char playBoard[BOARD_SIZE] = { CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY };
char borderedBoard[58] = { FLAG_PRINT_STRING,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF,
	BORDER_VERTICAL,CELL_EMPTY,BORDER_VERTICAL,CELL_EMPTY, BORDER_VERTICAL,CELL_EMPTY, BORDER_VERTICAL ,CRLF,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF,
	BORDER_VERTICAL,CELL_EMPTY,BORDER_VERTICAL,CELL_EMPTY, BORDER_VERTICAL,CELL_EMPTY, BORDER_VERTICAL ,CRLF,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF,
	BORDER_VERTICAL,CELL_EMPTY,BORDER_VERTICAL,CELL_EMPTY, BORDER_VERTICAL,CELL_EMPTY, BORDER_VERTICAL , CRLF,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF
};

char borderedPos[58] = { FLAG_PRINT_STRING,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF,
	BORDER_VERTICAL,ONE,BORDER_VERTICAL,TWO, BORDER_VERTICAL,THREE, BORDER_VERTICAL ,CRLF,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF,
	BORDER_VERTICAL,FOUR,BORDER_VERTICAL,FIVE, BORDER_VERTICAL,SIX, BORDER_VERTICAL ,CRLF,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF,
	BORDER_VERTICAL,SEVEN,BORDER_VERTICAL,EIGHT, BORDER_VERTICAL,NINE, BORDER_VERTICAL , CRLF,
	BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL, BORDER_HORIZONTAL,BORDER_HORIZONTAL,CRLF
};
int board_mapping[BOARD_SIZE] = {9,11,13,25,27,29,41,43,45};
int currPlayer;
int cellCounter = 0;
player_t *currentPlayer;


// mark both play 
void paintBoard(int pos, char sign) 
{
	char *boardData = &borderedBoard[1];
	boardData[board_mapping[pos]] = sign;

	boardData = &borderedPos[1];
	boardData[board_mapping[pos]] = 0x2F;
}


int ApplyCell(char cell, char sign)
{
	int cellPos = cell - 0x30 - 1;	//extract the numeric value from ASCII presentation

	//if user entered cell outof range or used 
	if (cellPos >= 0 && cellPos < BOARD_SIZE && playBoard[cellPos] == CELL_EMPTY)
	{
		playBoard[cellPos] = sign;
		paintBoard(cellPos, sign);
		cellCounter++;

		// if game is over
		if (cellCounter == BOARD_SIZE)
		{
			return PLAYER_STATE_EXIT;
		}

		return PLAYER_STATE_NEXT;
	}


	return PLAYER_STATE_STEP_ERR;

}

int getCurrentPlayer()
{
	return currPlayer;
}

void setCurrentPlayer(player_t *player) {
	currentPlayer = player;
}

void switchPlayer() {
	if (currPlayer == PLAYER_A)
		currPlayer = PLAYER_B;
	else
		currPlayer = PLAYER_A;
}



const char* getMsg(int msgId) {
	if (msgId == MSG_PRINT_BOARD)
		return borderedBoard;
	if (msgId == MSG_PRINT_AVAILABLE)
		return borderedPos;

	return msg_table[msgId];
}

void setFlag(int msgId, char flag)
{
	msg_table[msgId][0] = flag;
}