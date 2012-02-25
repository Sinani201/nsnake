#include <stdio.h>
#include <stdlib.h>
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

struct body
{
	int nextDir; 
	int x;
	int y;
	char sprite;
};

int i;

void redraw(struct body *snake,int foodx, int foody)
{
	erase();	// Clears the screen

	mvaddch(foody,foodx,'*');	// Draw food

	mvaddch(snake[0].y,snake[0].x,'@');	// Draw the snake head

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
		if(snake[0].nextDir != DOWN)
		{
			snake[0].nextDir = UP;
		}
	}
	else if(ch == KEY_DOWN || ch == 's')
	{
		if(snake[0].nextDir != UP)
		{
			snake[0].nextDir = DOWN;

		}
	}
	else if(ch == KEY_LEFT || ch == 'a')
	{
		if(snake[0].nextDir != RIGHT)
		{
			snake[0].nextDir = LEFT;
		}
	}
	else if(ch == KEY_RIGHT || ch == 'd')
	{
		if(snake[0].nextDir != LEFT)
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
		*outx = rand() % 10;
		*outy = rand() % 10;

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

	struct body * snake;
	snake = (struct body *) calloc(10,sizeof(struct body));

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

				if(i != 0)
				{
					// Set the nextDir for the next body in the chain
					snake[i].nextDir = snake[i-1].nextDir;
				}
			}

			// Count again to see if the snake has hit its tail
			for(i = 1; i < snakelength; i++)
			{
				if(snake[i].x == snake[0].x && snake[i].y == snake[0].y)
				{
					gameover = true;
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
		if(now-fpstick > 1 / FPS)
		{
			redraw(snake,foodx,foody);
			fpstick = clock();
		}

		// Is the game over?
		if(gameover)
		{
			break;
		}
	}
	free(snake);
}
