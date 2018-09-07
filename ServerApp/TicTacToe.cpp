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
int currPlayer = PLAYER_A;
int cellCounter = 0;
player_t *winner = NULL;


// mark both boards with play step
void paintBoard(int pos, char sign) 
{
	char *boardData = &borderedBoard[1];
	boardData[board_mapping[pos]] = sign;

	boardData = &borderedPos[1];
	boardData[board_mapping[pos]] = 0x2F;

	//checkForWinning();
}

//sum horizontal, vertical and diagonal cells to check for a match
int checkForWinning() 
{
	char cells = 0;
	int pos;
	int win = 0;
	char *boardData = &borderedBoard[1];

	if (cellCounter == 0) // beginning of the game
		return win;

	// horizontal
	if (playBoard[0] != CELL_EMPTY && playBoard[0] == playBoard[1] && playBoard[0] == playBoard[2])
		win = 1;
	else if (playBoard[3] != CELL_EMPTY && playBoard[3] == playBoard[4] && playBoard[3] == playBoard[5])
		win = 1;
	else if (playBoard[6] != CELL_EMPTY && playBoard[6] == playBoard[7] && playBoard[6] == playBoard[8])
		win = 1;
	// vertical
	if (playBoard[0] != CELL_EMPTY && playBoard[0] == playBoard[3] && playBoard[0] == playBoard[6])
		win = 1;
	else if (playBoard[1] != CELL_EMPTY && playBoard[1] == playBoard[4] && playBoard[1] == playBoard[7])
		win = 1;
	else if (playBoard[2] != CELL_EMPTY && playBoard[2] == playBoard[5] && playBoard[2] == playBoard[8])
		win = 1;
	// diagonal
	if (playBoard[0] != CELL_EMPTY && playBoard[0] == playBoard[4] && playBoard[0] == playBoard[8])
		win = 1;
	else if (playBoard[2] != CELL_EMPTY && playBoard[2] == playBoard[4] && playBoard[2] == playBoard[6])
		win = 1;

	
	return win;
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

		// if we have a winner
		if (checkForWinning() == 1)
		{
			return PLAYER_STATE_WINNER;
		}

		// if game is over
		if (cellCounter == BOARD_SIZE)
		{
			return PLAYER_STATE_GAMEOVER;
		}

		return PLAYER_STATE_NEXT;
	}


	return PLAYER_STATE_STEP_ERR;

}

int getCurrentPlayer()
{
	return currPlayer;
}

void setLastPlayer(player_t *player) {
	winner = player;
}

player_t * getLastPlayer()
{
	return winner;
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

char* getWritableMsg(int msgId)
{
	return msg_table[msgId];
}

void setFlag(int msgId, char flag)
{
	msg_table[msgId][0] = flag;
}