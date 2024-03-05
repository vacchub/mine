#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mine.h"

struct handle handle;

/********************************************************************
 * main()
 *******************************************************************/
int main(int argc, char *argv[])
{
	int level;

	if (argc != 2)
	{
		printf("%s level(1~3)\n", argv[0]);
		return 0;
	}

	level = atoi(argv[1]);
	if (l_init_handle(level) < 0)
		return 0;
	l_init_board();
	screen_show();

	return 0;
}
