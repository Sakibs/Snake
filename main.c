#include <ncurses.h>
#include <stdlib.h>
#include <time.h> // necessary to seed rand()

#define TICKRATE 150

#define WORLD_WIDTH 50
#define WORLD_HEIGHT 20

#define SNAKEY_MAX_LENGTH 40
#define SNAKEY_MIN_LENGTH 5

enum direction {UP, DOWN, RIGHT, LEFT};
enum gamestate {PLAY, PAUSE, LOST};

int game_state;

typedef struct coord{
	int x;
	int y;
} Food;

typedef struct spart{
	int x;
	int y;
} snakeypart;

typedef struct SnakeData {
	int cur_snakey_length;
	snakeypart body[SNAKEY_MAX_LENGTH];
} Snake;

int check_collision(Snake *snakey);
int setup_snakey(Snake* snakey, int begx, int begy);
int update_snakey_pos(Snake *snakey, int direction);
void grow_snakey(Snake *snakey);
int update_screen(WINDOW *win, Snake *snakey, int direction, Food *byte);
int update_menu(WINDOW* menu, int message);

void set_food_coord(Snake *Snakey, Food *byte);
int check_ate_food(Snake *snakey, Food *byte);

int main()
{
    // Randomize the seed
    srand( (unsigned int)time(NULL) );

    WINDOW *snakey_world;
	WINDOW *menu_win;
	//hide the cursor, we don't want to see it	
    int offsetx, offsety, menu_offsety, ch;

    initscr();
	noecho();
	cbreak();
	timeout(TICKRATE);
	keypad(stdscr, true);
	curs_set(0); //hide the cursor

	
	//printw("Snake v 1.1 - Press p to play/pause, x to quit\n");
    refresh();

    offsetx = (COLS - WORLD_WIDTH)/2;
    offsety = (LINES - WORLD_HEIGHT)/2;
	menu_offsety = (offsety - 4);

    snakey_world = newwin(WORLD_HEIGHT, WORLD_WIDTH, offsety, offsetx);

	//draw the menu window
	menu_win = newwin(4, WORLD_WIDTH, menu_offsety, offsetx);
	update_menu(menu_win, 0);

	//Declare and setup snake and food
	Snake snakey;
	Food byte;
	
	int sbegx = WORLD_WIDTH - (WORLD_WIDTH - SNAKEY_MIN_LENGTH)/2 -1;
	int sbegy = (WORLD_HEIGHT - 1)/2;
	setup_snakey(&snakey, sbegx, sbegy);
	set_food_coord(&snakey, &byte);
	
	//setup game variables
	int cur_dir = RIGHT;
	game_state = PLAY;
	
	while((ch = getch()) != 'x'){
		update_screen(snakey_world, &snakey, cur_dir, &byte);
		switch(game_state){
			case PLAY:
				if(ch != ERR){
					switch(ch){
						//if were moving make sure we dont move back to collide with the
						//part right behind the head
						case KEY_UP:
							if(cur_dir != DOWN)
								cur_dir = UP;
							break;
						case KEY_DOWN:
							if(cur_dir != UP)
								cur_dir = DOWN;
							break;
						case KEY_RIGHT:
							if(cur_dir != LEFT)
								cur_dir = RIGHT;
							break;
						case KEY_LEFT:
							if(cur_dir != RIGHT)
								cur_dir = LEFT;
							break;
						case 'p':
							game_state = PAUSE;
							break;
						case 'r':
							setup_snakey(&snakey, sbegx, sbegy);
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
							setup_snakey(&snakey, sbegx, sbegy);
							set_food_coord(&snakey, &byte);
							game_state = PLAY;
							cur_dir = RIGHT;
							break;
						default:
							break;
					}
				}
				break;
			case LOST:
				update_menu(menu_win, 1);
				
				if(ch != ERR){
					switch (ch) {
						case 'r': //reset snakey and then reset the screen
							update_menu(menu_win, 0);
							setup_snakey(&snakey, sbegx, sbegy);
							set_food_coord(&snakey, &byte);
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
	}
	
    delwin(snakey_world);
    endwin();
    return 0;
}


int setup_snakey(Snake *snakey, int begx, int begy){
	snakey->cur_snakey_length = SNAKEY_MIN_LENGTH;
	int i;
	
	//set up initial snakey body positions
	for (i = 0; i<SNAKEY_MIN_LENGTH; i++){
		snakey->body[i].x = begx-i;
		snakey->body[i].y = begy;
	}
	//set the rest of the body values to 0 so no junk values remain
	for(; i<SNAKEY_MAX_LENGTH; i++){
		snakey->body[i].x = 0;
		snakey->body[i].y = 0;
	}
	return 0;
}

int update_screen(WINDOW *win, Snake *snakey, int direction, Food *byte){
	wclear(win);
	
	int i;
	int cur_snakey_len = snakey->cur_snakey_length;
	
	if(game_state == PLAY)
		update_snakey_pos(snakey, direction);
	
	for(i=1; i<cur_snakey_len; i++)
		mvwaddch(win, snakey->body[i].y, snakey->body[i].x, '#');
	
	int collided = 0;
	//check if snake ate food
	if(check_ate_food(snakey, byte) == 1){
		mvwaddch(win, snakey->body[0].y, snakey->body[0].x, 'O');
		set_food_coord(snakey, byte);
		grow_snakey(snakey);
		beep();
		collided = 1;
	}
	else{
		mvwaddch(win, byte->y, byte->x, '@');
	}	
	//check if player caused snakey to collide, update game_state if the head
	//of the snake intersects with any other parts of the snake.
	if(check_collision(snakey) == 1){
		mvwaddch(win, snakey->body[0].y, snakey->body[0].x, 'X');
		game_state = LOST;
		collided = 1;
		//flash();
	}
	
	if(!collided)
		mvwaddch(win, snakey->body[0].y, snakey->body[0].x, '#');

	box(win, 0, 0);	
	wrefresh(win);
	return 0;
}

int update_snakey_pos(Snake *snakey, int direction){
	int i;
	int cur_snakey_len = snakey->cur_snakey_length;
	
	for(i=cur_snakey_len-1; i>0 ; i--){
		snakey->body[i] = snakey->body[i-1];
	}
	
	int x = snakey->body[0].x;
	int y = snakey->body[0].y;
	//check the direction of the snake and update coordinates of head accordingly
	//wrap around if at border
	switch(direction){
		case UP:
			y-1 == 0 ? y = WORLD_HEIGHT-2 : y--;
			break;
		case DOWN:
			y+1 == WORLD_HEIGHT-1 ? y = 1 : y++;
			break;
		case RIGHT:
			x+1 == WORLD_WIDTH-1 ? x = 1 : x++;
			break;
		case LEFT: 
			x-1 == 0 ? x = WORLD_WIDTH-2 : x--;
			break;
		default: 
			break;
	}
	snakey->body[0].x = x;
	snakey->body[0].y = y;
	return 0;
}

void grow_snakey(Snake* snakey){
	if(snakey->cur_snakey_length < SNAKEY_MAX_LENGTH){
		snakey->cur_snakey_length++;
		
		//make the coordinates of the new part the coordinates of the last part in snakey
		int new_part = snakey->cur_snakey_length;
		snakey->body[new_part] = snakey->body[new_part-1];
	}
}

int check_collision(Snake *snakey){
	int headx = snakey->body[0].x;
	int heady = snakey->body[0].y;
	
	int cur_len = snakey->cur_snakey_length;
	//make sure head of snakey does not intersect with any othe part of the body
	int i;
	for(i = 1; i<cur_len; i++)
		if(snakey->body[i].x == headx && snakey->body[i].y == heady)
			return 1;
	
	return 0;
}

int check_ate_food(Snake *snakey, Food *byte){
	if(snakey->body[0].x == byte->x && snakey->body[0].y == byte->y)
		return 1;
	return 0;
}

int update_menu(WINDOW *menu, int message){
	wclear(menu);
	switch(message){
		case 1: //the game over screen
			wprintw(menu, "Game Over...\nPress x to quit, r to reset.\n");
			break;
		default:
			wprintw(menu, "Snake v 1.3\nPress p to play/pause, x to quit, r to reset");
			break;
	}
	wrefresh(menu);
	return 0;
}

void set_food_coord(Snake *snakey, Food* byte){
	int clear = 1;
	int newx, newy;
	/*	generate new coord for food, if it is intersecting with the coord of one of the 
		snake body parts, keep trying until we get a valid coordinate*/
	do{
		//reset clear if it was set to 0 on last iteration
		if(clear == 0)
			clear = 1;
			
		newx = rand() % WORLD_WIDTH;
		newy = rand() % WORLD_HEIGHT;
		//check if newx or newy are on the border of the box, if they are
		//just increment them or decrement them.
		if(newx == 0)
			newx++;
		else if(newx == 49)
			newx--;
		if(newy == 0)
			newy++;
		else if(newy == 49)
			newy--;
		
		int snakey_len = snakey->cur_snakey_length;
		int i;
		
		for(i=0; i<snakey_len; i++)
			if(snakey->body[i].x == newx && snakey->body[i].y == newy)
				clear = 0;		
	}while(clear != 1);
	
	byte->x = newx;
	byte->y = newy;
}

