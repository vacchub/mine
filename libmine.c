#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "mine.h"

struct  lvtbl {
	int		level;
	int		row;
	int		column;
	int		mine;
} lvtbl[] = {
	{  1,  9,  9, 10	},
	{  2, 16, 16, 40	},
	{  3, 16, 30, 99	},
	{ -1,  0,  0,  0	}
};

extern struct handle handle;

/********************************************************************
 * l_init_handle()
 *******************************************************************/
int l_init_handle(int level)
{
	handle.level = level;
	if (handle.level < 1 || handle.level > 3)
		return -1;

	handle.max_row = lvtbl[handle.level - 1].row;
	handle.max_column = lvtbl[handle.level - 1].column;
	handle.max_mine = lvtbl[handle.level - 1].mine;
	handle.isavaild = 0;
	return 0;
}

/********************************************************************
 * _shuffle()
 *******************************************************************/
void _shuffle(int *arr, int num)
{
	int temp;
	int ii, rn;

	srand(time(NULL));  
	for (ii = 0; ii < num-1; ii++)
	{
		rn = rand() % (num - ii) + ii;
		temp = arr[ii];
		arr[ii] = arr[rn];
		arr[rn] = temp;
	}
}

/********************************************************************
 * _set_number()
 *******************************************************************/
void _set_number()
{
	int ii, jj, kk, xx, yy, num;

	for (ii = 0; ii < handle.max_row; ii++)
	{
		for (jj = 0; jj < handle.max_column; jj++)
		{
			num = 0;

			if (handle.board[ii][jj].value == VALUE_MINE)
				continue;
			
			for (kk = 0; kk < 8; kk++)
			{
				switch (kk)
				{
				case 0: xx = ii-1;		yy = jj-1;		break;
				case 1: xx = ii-1;		yy = jj;		break;
				case 2: xx = ii-1;		yy = jj+1;		break;
				case 3: xx = ii;		yy = jj-1;		break;
				case 4: xx = ii;		yy = jj+1;		break;
				case 5: xx = ii+1;		yy = jj-1;		break;
				case 6: xx = ii+1;		yy = jj;		break;
				case 7: xx = ii+1;		yy = jj+1;		break;
				}

				if ((0 <= xx && xx < handle.max_row) && (0 <= yy && yy < handle.max_column))
				{
					if (handle.board[xx][yy].value == VALUE_MINE)
						num++;
				}
			}

			if (num != 0)
			{
				handle.board[ii][jj].value = num + '0';	
			}
		}
	}
}

/********************************************************************
 * l_init_board()
 *******************************************************************/
int l_init_board()
{
	int random_mine[MAX_ROW * MAX_ROW];
	int ii, jj, xx, yy, index=0;

	memset(handle.board, 0x00, sizeof(handle.board));
	memset(random_mine, 0x00, sizeof(random_mine));

	for (ii = 0; ii < handle.max_row; ii++)
	{
		for (jj = 0; jj < handle.max_column; jj++)
		{
			handle.board[ii][jj].show = SHOW_N;
			handle.board[ii][jj].value = VALUE_EMPTY;
			random_mine[ii * handle.max_column + jj] = index;
			index++;
		}
	}

	/* create mine		*/
	_shuffle(&random_mine[0], handle.max_row * handle.max_column);

	for (ii = 0; ii < handle.max_mine; ii++)
	{
		index = random_mine[ii];
		xx = index / handle.max_column;
		yy = index % handle.max_column;
		handle.board[xx][yy].value = VALUE_MINE;
	}

	/* setting number	*/
	_set_number();

	handle.isrunning = 1;
	return 0;
}

/********************************************************************
 * l_isend()
 *******************************************************************/
int l_isend()
{
	int ii, jj;

	for (ii = 0; ii < handle.max_row; ii++)
	{
		for (jj = 0; jj < handle.max_column; jj++)
		{
			if (handle.board[ii][jj].show == SHOW_N)
				return 0;

			if (handle.board[ii][jj].value == VALUE_MINE && handle.board[ii][jj].show != SHOW_T)
				return 0;

			if (handle.board[ii][jj].value != VALUE_MINE && handle.board[ii][jj].show == SHOW_T)
				return 0;
		}
	}

	return 1;
}

/********************************************************************
 * l_check_availd()
 *******************************************************************/
int l_check_availd(int x, int y)
{
	int xx, yy, kk;

	if (handle.isavaild == 0)
		return 0;

	for (kk = 0; kk < 8; kk++)
	{
		switch (kk)
		{
		case 0: xx = handle.availd.x-1;      yy = handle.availd.y-1;      break;
		case 1: xx = handle.availd.x-1;      yy = handle.availd.y;        break;
		case 2: xx = handle.availd.x-1;      yy = handle.availd.y+1;      break;
		case 3: xx = handle.availd.x;        yy = handle.availd.y-1;      break;
		case 4: xx = handle.availd.x;        yy = handle.availd.y+1;      break;
		case 5: xx = handle.availd.x+1;      yy = handle.availd.y-1;      break;
		case 6: xx = handle.availd.x+1;      yy = handle.availd.y;        break;
		case 7: xx = handle.availd.x+1;      yy = handle.availd.y+1;      break;
		}

		if ((0 <= xx && xx < handle.max_row) && (0 <= yy && yy < handle.max_column))
		{
			if (xx == x && yy == y)
				return 1;
		}
	}

	return 0;
}

/********************************************************************
 * _click_spread()
 *******************************************************************/
void _click_spread(struct pos *origin, struct pos *pos)
{
	struct pos t_pos;
	int xx, yy, kk;

	if (handle.board[pos->x][pos->y].value == VALUE_MINE)
	{
		return;
	}
	else if (VALUE_1 <= handle.board[pos->x][pos->y].value && 
		handle.board[pos->x][pos->y].value <= VALUE_8)
	{
		if (memcmp(origin, pos, sizeof(struct pos)) != 0)
		{
			handle.board[pos->x][pos->y].show = SHOW_Y;
			return;
		}
	}

	handle.board[pos->x][pos->y].show = SHOW_Y;
	for (kk = 0; kk < 8; kk++)
	{
		switch (kk)
		{
		case 0: xx = pos->x-1;		yy = pos->y-1;		break;
		case 1: xx = pos->x-1;		yy = pos->y;		break;
		case 2: xx = pos->x-1;		yy = pos->y+1;		break;
		case 3: xx = pos->x;		yy = pos->y-1;		break;
		case 4: xx = pos->x;		yy = pos->y+1;		break;
		case 5: xx = pos->x+1;		yy = pos->y-1;		break;
		case 6: xx = pos->x+1;		yy = pos->y;		break;
		case 7: xx = pos->x+1;		yy = pos->y+1;		break;
		}

		if ((0 <= xx && xx < handle.max_row) && (0 <= yy && yy < handle.max_column))
		{
			t_pos.x = xx;
			t_pos.y = yy;
			if (handle.board[t_pos.x][t_pos.y].show == SHOW_Y)
				continue;
			_click_spread(origin, &t_pos);
		}
	}
}

/********************************************************************
 * _click_check()
 *******************************************************************/
void _click_check(struct pos *pos)
{
	int xx, yy, kk, number, num=0;

	for (kk = 0; kk < 8; kk++)
	{
		switch (kk)
		{
		case 0: xx = pos->x-1;		yy = pos->y-1;		break;
		case 1: xx = pos->x-1;		yy = pos->y;		break;
		case 2: xx = pos->x-1;		yy = pos->y+1;		break;
		case 3: xx = pos->x;		yy = pos->y-1;		break;
		case 4: xx = pos->x;		yy = pos->y+1;		break;
		case 5: xx = pos->x+1;		yy = pos->y-1;		break;
		case 6: xx = pos->x+1;		yy = pos->y;		break;
		case 7: xx = pos->x+1;		yy = pos->y+1;		break;
		}

		if ((0 <= xx && xx < handle.max_row) && (0 <= yy && yy < handle.max_column))
		{
			if (handle.board[xx][yy].show == SHOW_T)
				num++;
		}
	}

	number = handle.board[pos->x][pos->y].value - '0';
	if (number <= num)
	{
		for (kk = 0; kk < 8; kk++)
		{
			switch (kk)
			{
			case 0: xx = pos->x-1;		yy = pos->y-1;		break;
			case 1: xx = pos->x-1;		yy = pos->y;		break;
			case 2: xx = pos->x-1;		yy = pos->y+1;		break;
			case 3: xx = pos->x;		yy = pos->y-1;		break;
			case 4: xx = pos->x;		yy = pos->y+1;		break;
			case 5: xx = pos->x+1;		yy = pos->y-1;		break;
			case 6: xx = pos->x+1;		yy = pos->y;		break;
			case 7: xx = pos->x+1;		yy = pos->y+1;		break;
			}

			if ((0 <= xx && xx < handle.max_row) && (0 <= yy && yy < handle.max_column))
			{
				if (handle.board[xx][yy].show == SHOW_T && handle.board[xx][yy].value != VALUE_MINE)
				{
					handle.isrunning = 0;
					return;
				}
			}
		}

		if (number == num)
		{
			_click_spread(pos, pos);
			return;
		}
	}

	handle.isavaild = 1;
	handle.availd.x = pos->x;
	handle.availd.y = pos->y;
}

/********************************************************************
 * l_key_proc()
 *******************************************************************/
int l_key_proc(struct pos *cursor, int key)
{
	switch (key)
	{
	case MKEY_UP:
		if (cursor->x - 1 < 0)
			break;
		cursor->x -= 1;
		break;
	case MKEY_DOWN:
		if (cursor->x + 1 >= handle.max_row)
			break;
		cursor->x += 1;
		break;
	case MKEY_LEFT:
		if (cursor->y - 1 < 0)
			break;
		cursor->y -= 1;
		break;
	case MKEY_RIGHT:
		if (cursor->y + 1 >= handle.max_column)
			break;
		cursor->y += 1;
		break;
	case MKEY_CLICK_HARD:
		if (handle.board[cursor->x][cursor->y].show == SHOW_Y && 
				VALUE_1 <= handle.board[cursor->x][cursor->y].value &&
				handle.board[cursor->x][cursor->y].value <= VALUE_8)
		{
			_click_check(cursor);
			break;
		}

		if (handle.board[cursor->x][cursor->y].value == VALUE_MINE)
		{
			handle.isrunning = 0;
			break;
		}
		else if (VALUE_1 <= handle.board[cursor->x][cursor->y].value && 
			handle.board[cursor->x][cursor->y].value <= VALUE_8)
		{
			handle.board[cursor->x][cursor->y].show = SHOW_Y;
			break;
		}

		_click_spread(cursor, cursor);
		break;
	case MKEY_CLICK_SOFT:
		if (handle.board[cursor->x][cursor->y].show == SHOW_T)
			handle.board[cursor->x][cursor->y].show = SHOW_N;
		else if (handle.board[cursor->x][cursor->y].show == SHOW_N)
			handle.board[cursor->x][cursor->y].show = SHOW_T;
		break;
	}
	return 0;
}

