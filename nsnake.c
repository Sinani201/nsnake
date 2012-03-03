#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#define FPS 50
#define MAXSNAKE 500

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

clock_t speedtick,now,fpstick;
int snakelength;
bool walls;
int screenw;
int screenh;

struct body
{
	int nextDir; 
	// Current dir is used to ensure
	// that the snake head doesn't run
	// into itself
	int currentDir;
	int x;
	int y;
	char sprite;
};

int i;

void drawWalls()
{
	mvaddch(0,0,'+');
	mvaddch(screenh,0,'+');
	mvaddch(0,screenw,'+');
	mvaddch(screenh,screenw,'+');

	for(int i=1; i < screenw; i++)
	{
		mvaddch(0,i,'-');
		mvaddch(screenh,i,'-');
	}
	for(int i=1; i < screenh; i++)
	{
		mvaddch(i,0,'|');
		mvaddch(i,screenw,'|');	
	}
}

void redraw(struct body *snake,int foodx, int foody)
{
	erase();	// Clears the screen
	
	drawWalls();
	
	mvaddch(foody,foodx,'*');	// Draw food

	for(int i = 0; i < snakelength; i++)
	{
		mvaddch(snake[i].y,snake[i].x,snake[i].sprite);
	}

	refresh();
}

void readInput(struct body *snake)
{
	int ch;
	ch = getch(); // get key pressed
	if(ch == KEY_UP || ch == 'w')
	{
		// The snake cannot move in the opposite direction
		if(snake[0].currentDir != DOWN)
		{
			snake[0].nextDir = UP;
		}
	}
	else if(ch == KEY_DOWN || ch == 's')
	{
		if(snake[0].currentDir != UP)
		{
			snake[0].nextDir = DOWN;
		}
	}
	else if(ch == KEY_LEFT || ch == 'a')
	{
		if(snake[0].currentDir != RIGHT)
		{
			snake[0].nextDir = LEFT;
		}
	}
	else if(ch == KEY_RIGHT || ch == 'd')
	{
		if(snake[0].currentDir != LEFT)
		{
			snake[0].nextDir = RIGHT;
		}
	}
}

void plantFood(int *outx, int *outy, struct body *snake)
{
	// Get a random value for food
	//int screenx, screeny;	// Variables to use if a max screen size is defined
	for(int i = 0; i < snakelength; i++)
	{
		*outx = (rand() % (screenw-1)) + 1;
		*outy = (rand() % (screenh-1)) + 1;

		// If the food is hitting the snake
		if(*outx == snake[i].x && *outy == snake[i].y)
		{
			// Reset the counter
			i = 0;
		} else {
			// If the food is in an empty square, exit
			break;
		}
	}
	
}

int main(int argc, char **argv)
{
	// Curses initialization and stuff
	initscr();
	nonl();
	cbreak();
	nodelay(stdscr,true);
	srand(time(NULL));

	screenw = 11;
	screenh = 11;

	// Command line arguments
	if(argc == 2 && strcmp(argv[1], "-w") == 0)
	{
		// Enable walls
		walls = true;
	}

	struct body * snake;
	snake = (struct body *) calloc(MAXSNAKE,sizeof(struct body));

	fpstick = clock();
	speedtick = clock();
	int speed = 5;
	int foodx;
	int foody;

	// Initialize the snake
	snakelength = 1;
	snake[0].x = 5;
	snake[0].y = 5;
	snake[0].nextDir = 3;
	snake[0].sprite = '@';

	plantFood(&foodx, &foody, snake);
	bool gameover = false;

	while(1==1)
	{
		//time(&now);
		now = clock();

		readInput(snake);

		// speed is determined by the timer
		// if it has been fulfulled, move the snake
		if(now-speedtick > CLOCKS_PER_SEC / (snakelength+3) * 1.5)
		{
			for(i=snakelength-1; i>=0; i--)
			{
				switch(snake[i].nextDir)
				{
				case UP:
					snake[i].y--;
					break;
	
				case DOWN:
					snake[i].y++;
					break;
	
				case LEFT:
					snake[i].x--;
					break;
	
				case RIGHT:
					snake[i].x++;
					break;
				}
				
				snake[i].currentDir = snake[i].nextDir;

				if(i != 0)
				{
					// Set the nextDir for the next body in the chain
					snake[i].nextDir = snake[i-1].nextDir;
				}
			}

			// Count again to see if the snake has hit its tail
			for(i = 1; i < snakelength; i++)
			{
				if(snake[0].x == snake[0].x && snake[0].y == snake[0].y)
				{
					gameover = true;
				}
			}

			if(walls)
			{
				if(snake[0].x == 0 || snake[0].x == screenw
					|| snake[0].y == 0 || snake[0].y == screenh)
				{
					gameover = true;
				}
			} else {
				for(int i = 0; i < snakelength; i++)
				{
					if(snake[i].x == 0)
					{
						snake[i].x = screenw - 1;
					} else if(snake[i].x == screenw)
					{
						snake[i].x = 1;
					}
					if(snake[i].y == 0)
					{
						snake[i].y = screenh - 1;
					} else if(snake[i].y == screenh)
					{
						snake[i].y = 1;
					}
				}
			}
			
			// If the snake collides with food
			if(snake[0].x == foodx && snake[0].y == foody)
			{
				// Grow the snake
				switch(snake[snakelength-1].nextDir)
				{
				case UP:
					snake[snakelength].y = snake[snakelength-1].y+1;
					snake[snakelength].x = snake[snakelength-1].x;
					break;

				case DOWN:
					snake[snakelength].y = snake[snakelength-1].y-1;
					snake[snakelength].x = snake[snakelength-1].x;
					break;

				case LEFT:
					snake[snakelength].x = snake[snakelength-1].x+1;
					snake[snakelength].y = snake[snakelength-1].y;
					break;

				case RIGHT:
					snake[snakelength].x = snake[snakelength-1].x-1;
					snake[snakelength].y = snake[snakelength-1].y;
					break;
				}
				snake[snakelength].nextDir = snake[snakelength-1].nextDir;
				snake[snakelength].sprite = '#';
				snakelength++;
				plantFood(&foodx,&foody,snake);
			}

			speedtick = clock();
		}

		// handle timer loops
		//if(now-fpstick > CLOCKS_PER_SEC / FPS)
		//{
			redraw(snake,foodx,foody);
			fpstick = clock();
		//}

		// Is the game over?
		if(gameover)
		{
			break;
		}
	}
	free(snake);
	return(0);
}
