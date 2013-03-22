#include <ncurses.h>

#define TICKRATE 130

#define WORLD_WIDTH 50
#define WORLD_HEIGHT 20

#define SNAKEY_LENGTH 40
#define SNAKEY_MIN_LENGTH 4

enum direction {UP, DOWN, RIGHT, LEFT};
enum gamestate {PLAY, PAUSE, LOST};

int game_state;

typedef struct spart{
	int x;
	int y;
} snakeypart;

struct Snake{
	int cur_snakey_length;
	snakeypart body[SNAKEY_LENGTH];
};

int check_collision(snakeypart snakey[]);
int setup_snakey(snakeypart snakey[], int begx, int begy);
int update_snakey_pos(snakeypart snakey[], int direction);
int move_snakey(WINDOW *win, int direction, snakeypart snakey[]);


int main()
{
    WINDOW *snakey_world;
	WINDOW *menu_win;
    int offsetx, offsety, menu_offsety, i, ch;

    initscr();
	noecho();
	cbreak();
	timeout(TICKRATE);
	keypad(stdscr, true);
	
	//printw("Snake v 1.1 - Press p to play/pause, x to quit\n");
    refresh();

    offsetx = (COLS - WORLD_WIDTH)/2;
    offsety = (LINES - WORLD_HEIGHT)/2;
	menu_offsety = (offsety - 5);

    snakey_world = newwin(WORLD_HEIGHT, WORLD_WIDTH, offsety, offsetx);

	//draw the menu window
	menu_win = newwin(5, WORLD_WIDTH, menu_offsety, offsetx);
	wprintw(menu_win, "Snake v 1.1 - Press p to play/pause, x to quit\n");
	wrefresh(menu_win);
	
	//snakey that is displayed on the screen is displayed here.
	snakeypart snakey[SNAKEY_LENGTH];
	
	int sbegx = WORLD_WIDTH - (WORLD_WIDTH - SNAKEY_LENGTH)/2 -1;
	int sbegy = (WORLD_HEIGHT - 1)/2;
	
	setup_snakey(snakey, sbegx, sbegy);
	
	int cur_dir = RIGHT;
	game_state = PLAY;
	
	while((ch = getch()) != 'x'){
		move_snakey(snakey_world, cur_dir, snakey);
		switch(game_state){
			case PLAY:
				if(ch != ERR){
					switch(ch){
						case KEY_UP:
							cur_dir = UP;
							break;
						case KEY_DOWN:
							cur_dir = DOWN;
							break;
						case KEY_RIGHT:
							cur_dir = RIGHT;
							break;
						case KEY_LEFT:
							cur_dir = LEFT;
							break;
						case 'p':
							game_state = PAUSE;
							break;
						case 'r':
							setup_snakey(snakey, sbegx, sbegy);
							cur_dir = RIGHT;
							break;
						default:
							break;
					}
				}
				break;
			case PAUSE:
				if(ch != ERR){
					switch(ch){
						case 'p':
							game_state = PLAY;
							break;
						case 'r':
							setup_snakey(snakey, sbegx, sbegy);
							game_state = PLAY;
							cur_dir = RIGHT;
							break;
						default:
							break;
					}
				}
				break;
			case LOST:
				wclear(menu_win);
				wprintw(menu_win, "Game Over...\nPress x to quit, r to reset.\n");
				wrefresh(menu_win);
				
				if(ch != ERR){
					switch (ch) {
						case 'r': //reset snakey and then reset the screen
							wclear(menu_win);
							wprintw(menu_win, "Snake v 1.1 - Press p to play/pause\nx to quit\nr to reset");
							wrefresh(menu_win);
							
							setup_snakey(snakey, sbegx, sbegy);
							game_state = PLAY;
							cur_dir = RIGHT;
							break;
						default:
							break;
					}
				}
				break;
			default: 
				break;

		}
		//move_snakey(snakey_world, cur_dir, snakey);

	}
	
    delwin(snakey_world);

    endwin();

    return 0;
}


int setup_snakey(snakeypart snakey[], int begx, int begy)
{
	int i;
	for (i = 0; i<SNAKEY_LENGTH; i++){
		snakey[i].x = begx-i;
		snakey[i].y = begy;
	}
	
	return 0;
}

int move_snakey(WINDOW *win, int direction, snakeypart snakey[]){
	wclear(win);
	
	int i;
	
	if(game_state == PLAY)
	{
		update_snakey_pos(snakey, direction);
	}
	
	for(i=1; i<SNAKEY_LENGTH; i++) {
		mvwaddch(win, snakey[i].y, snakey[i].x, '#');
	}
	
	//check if player caused snakey to collide, update game_state if the head
	//of the snake intersects with any other parts of the snake.
	if(check_collision(snakey) == 1)
	{
		mvwaddch(win, snakey[0].y, snakey[0].x, 'X');
		game_state = LOST;
		//flash();
	}
	else
		mvwaddch(win, snakey[0].y, snakey[0].x, '#');

	box(win, 0, 0);
	
	wrefresh(win);
	
	return 0;
}

int update_snakey_pos(snakeypart snakey[], int direction)
{
	int i;
	for(i=SNAKEY_LENGTH-1; i>0 ; i--){
		snakey[i] = snakey[i-1];
	}
	
	int x = snakey[0].x;
	int y = snakey[0].y;
	
	switch(direction){
		case UP:
			y-1 == 0 ? y = WORLD_HEIGHT - 2 : y--;
			break;
		case DOWN:
			y+1 == WORLD_HEIGHT - 1 ? y = 1 : y++;
			break;
		case RIGHT:
			x+1 == WORLD_WIDTH - 1 ? x = 1 : x++;
			break;
		case LEFT: 
			x-1 == 0 ? x = WORLD_WIDTH - 2 : x--;
			break;
		default: 
			break;
	}
	
	snakey[0].x = x;
	snakey[0].y = y;
}

int check_collision(snakeypart snakey[])
{
	int headx = snakey[0].x;
	int heady = snakey[0].y;
	
	int i;
	for(i = 1; i<SNAKEY_LENGTH; i++)
		if(snakey[i].x == headx && snakey[i].y == heady)
			return 1;
	
	return 0;
}
