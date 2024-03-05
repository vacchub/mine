#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include "mine.h"

#define SCREEN_NUM_DARK			1
#define SCREEN_NUM_EMPTY		2
#define SCREEN_NUM_CURSOR		3
#define SCREEN_NUM_MINE			4
#define SCREEN_NUM_AVAILD		5
#define SCREEN_NUM_FINISH		6

#define SHAPE_DARK				' '
#define SHAPE_EMPTY				' '
#define SHAPE_MINE				'!'

extern struct handle handle;

/********************************************************************
 * screen_msg()
 *******************************************************************/
void screen_msg(char *msg)
{
	move(5, 0);
	printw("%s", msg);
	refresh();
}

/********************************************************************
 * screen_help()
 *******************************************************************/
void screen_help()
{
	move(0, 0);
	printw("q     : quit");
	move(1, 0);
	printw("space : soft click");
	move(2, 0);
	printw("enter : hard click");
	refresh();
}

/********************************************************************
 * screen_cursor()
 *******************************************************************/
void screen_cursor(struct pos cursor)
{
	move(3, 0);
	printw("cursor(%2d, %2d)", cursor.x, cursor.y);
}

/********************************************************************
 * _mode_show()
 *******************************************************************/
void _mode_show(struct pos cursor, int xx, int yy)
{
	if (handle.isrunning == 0 && xx == cursor.x && yy == cursor.y)
	{
		attron(COLOR_PAIR(SCREEN_NUM_FINISH));
		printw("%c ", SHAPE_MINE);
	}
	else
	{
		if (handle.board[xx][yy].value == VALUE_EMPTY)
		{
			attron(COLOR_PAIR(SCREEN_NUM_EMPTY));
			printw("%c ", SHAPE_EMPTY);
		}
		else if (handle.board[xx][yy].value == VALUE_MINE)
		{
			attron(COLOR_PAIR(SCREEN_NUM_MINE));
			printw("%c ", SHAPE_MINE);
		}
		else
		{
			attron(COLOR_PAIR(SCREEN_NUM_EMPTY));
			printw("%c ", handle.board[xx][yy].value);
		}
	}
}

/********************************************************************
 * _mode_normal()
 *******************************************************************/
void _mode_normal(struct pos cursor, int xx, int yy)
{
	if (handle.board[xx][yy].show == SHOW_N)
	{
		attron(COLOR_PAIR(SCREEN_NUM_DARK));

		if (xx == cursor.x && yy == cursor.y)
			attron(COLOR_PAIR(SCREEN_NUM_CURSOR));

		if (l_check_availd(xx, yy))
			attron(COLOR_PAIR(SCREEN_NUM_AVAILD));

		printw("%c ", SHAPE_DARK);
	}
	else if (handle.board[xx][yy].show == SHOW_T)
	{
		attron(COLOR_PAIR(SCREEN_NUM_MINE));

		if (xx == cursor.x && yy == cursor.y)
			attron(COLOR_PAIR(SCREEN_NUM_CURSOR));
		printw("%c ", SHAPE_MINE);
	}
	else
	{
		if (handle.board[xx][yy].value == VALUE_MINE)
		{
			attron(COLOR_PAIR(SCREEN_NUM_MINE));
			if (xx == cursor.x && yy == cursor.y)
				attron(COLOR_PAIR(SCREEN_NUM_CURSOR));
		}
		else if (handle.board[xx][yy].value == VALUE_EMPTY)
		{
			attron(COLOR_PAIR(SCREEN_NUM_EMPTY));
			if (xx == cursor.x && yy == cursor.y)
				attron(COLOR_PAIR(SCREEN_NUM_CURSOR));
			printw("%c ", SHAPE_EMPTY);
		}
		else
		{
			attron(COLOR_PAIR(SCREEN_NUM_EMPTY));
			if (xx == cursor.x && yy == cursor.y)
				attron(COLOR_PAIR(SCREEN_NUM_CURSOR));
			printw("%c ", handle.board[xx][yy].value);
		}
	}
}

/********************************************************************
 * screen_show()
 *******************************************************************/
int screen_show()
{
	struct pos pos;
	struct pos cursor;
	int ii, jj, ch=0;

	initscr();
	start_color();
	init_pair(SCREEN_NUM_DARK, COLOR_WHITE, COLOR_BLUE);		/* DARK		*/
	init_pair(SCREEN_NUM_EMPTY, COLOR_BLACK, COLOR_YELLOW);		/* EMPTY	*/
	init_pair(SCREEN_NUM_CURSOR, COLOR_WHITE, COLOR_RED);		/* CURSOR	*/
	init_pair(SCREEN_NUM_MINE, COLOR_WHITE, COLOR_MAGENTA);		/* MINE		*/
	init_pair(SCREEN_NUM_AVAILD, COLOR_WHITE, COLOR_GREEN);		/* AVAILD	*/
	init_pair(SCREEN_NUM_FINISH, COLOR_WHITE, COLOR_RED);		/* FINISH	*/
	noecho();
	raw();

	screen_help();

	cursor.x = 0;	
	cursor.y = 0;

	do
	{
AVAILD_PROC:
		if (handle.isavaild == 0)
		{
			l_key_proc(&cursor, ch);
			if (handle.isrunning == 0)
			{
				ch = MKEY_SHOW;
			}
		}
		else
		{
			usleep(AVAILD_TIME);
			handle.isavaild = 0;
		}

		screen_cursor(cursor);
		pos.x = 7;
		pos.y = 0;

		for (ii = 0; ii < handle.max_row; ii++)
		{
			for (jj = 0; jj < handle.max_column; jj++)
			{
				move(pos.x, pos.y);
				if (ch == MKEY_SHOW)
				{
					_mode_show(cursor, ii, jj);
				}
				else
				{
					_mode_normal(cursor, ii, jj);
				}
				pos.y += 2;

				attroff(COLOR_PAIR(SCREEN_NUM_DARK));
				attroff(COLOR_PAIR(SCREEN_NUM_EMPTY));
				attroff(COLOR_PAIR(SCREEN_NUM_CURSOR));
				attroff(COLOR_PAIR(SCREEN_NUM_MINE));
				attroff(COLOR_PAIR(SCREEN_NUM_AVAILD));
				attroff(COLOR_PAIR(SCREEN_NUM_FINISH));
			}
			pos.x += 1;
			pos.y = 0;
		}
		printw("\n");

		if (handle.isrunning == 0)
		{
			screen_msg("Fail");
			break;
		}

		if (handle.isavaild)
		{
			refresh();
			goto AVAILD_PROC;
		}

		if (l_isend())
		{
			screen_msg("Success");
			break;
		}
	} while((ch = getch()) != MKEY_QUIT);

	refresh();
	endwin();
	return 0;
}
