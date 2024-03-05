#ifndef _MINE_H_
#define _MINE_H_

#define	MAX_ROW				30

#define MKEY_UP				65
#define MKEY_DOWN			66
#define MKEY_LEFT			68
#define MKEY_RIGHT			67
#define MKEY_CLICK_SOFT		32
#define MKEY_CLICK_HARD		10
#define MKEY_QUIT			113
#define MKEY_SHOW			115


#define SHOW_Y				'Y'	
#define SHOW_N				'N'	
#define SHOW_T				'T'	

#define VALUE_EMPTY			'E'	
#define VALUE_MINE			'M'
#define VALUE_1				'1'
#define VALUE_8				'8'

#define AVAILD_TIME			500000

struct pos {
	int		x;
	int		y;
};

struct board {
	char	show;			/* 'Y'       : open			*/
							/* 'N'       : close		*/
							/* 'T'       : temp mine	*/

	char	value;			/* 'E'       : EMPTY		*/
							/* '1' ~ '8' : NUMBER		*/
							/* 'M'       : MINE			*/
};

struct handle {
	struct	board	board[MAX_ROW][MAX_ROW];
	int		level;
	int		max_row;
	int		max_column;
	int		max_mine;
	int		isrunning;
	int		isavaild;
	struct	pos		availd;
	char	msg[1024];
};

int		l_init_handle(int level);
int		l_init_board();
int		l_key_proc(struct pos *cursor, int key);
int		l_check_availd(int x, int y);
int		l_isend();

int		screen_show();
void	screen_msg(char *msg);

#endif
