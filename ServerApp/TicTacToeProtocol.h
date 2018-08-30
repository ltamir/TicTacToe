#include "stdafx.h"
/*
The protocol consists of the following:
- 1st byte - instruction according to the flags bellow.
- rest of message: string (char array)
For each message from the server, the client returns either byte with 0x1 or other input
*/

#define FLAG_EXIT 1			// print message and exit app
#define FLAG_PRINT_CHAR 2	// print char and return 0x1
#define FLAG_PRINT_STRING 3	// print message and return 0x1
#define FLAG_READ_CHAR 4	// return a char from stdin
#define FLAG_READ_STRING 5	// return a string from stdin