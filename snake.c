/* PROGRAMMING EXAM PROJECT */

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>

#define ALT 20					/* land height */
#define LARG 70					/* land width */

#define B_V (char)186			/* charachter for the vertical boarder */
#define B_O (char)205			/* charachter for the orizzontal boarder */
#define z 	(char)219			/* charachter for the writings */
#define A_S (char)201			/* charachter for the angle on the top-left */
#define A_D (char)187			/* charachter for the angle on the top-right */
#define B_S (char)200			/* charachter for the angle on the bottom-left */
#define B_D (char)188			/* charachter for the angle on the bottom-right */

#define SNAKE_PIXEL (char)219         /* character for the snake body */
#define SNAKE_HEAD (char)177          /* character for the snake head */
#define ESC 0xE0			  		/* hexadecimal value that, if returned from the _getch() [windows.h] guarantees that an arrow key it's been pressed */

#define FRUIT (char)111               /* character for the fruit */
#define BONUS_FRUIT (char)169         /* character for the bonus fruit */

#define ALT_SCRITTA 5               /* writings height */

#define CLASSIFICA "rank.txt"     /* path for the ranking file */


typedef struct           		/* structure within the name and scores of the first 3 ranking players */
{
    char name_player[4];
    char name1[4], name2[4], name3[4];
    int score1, score2, score3;
}Classifica;

typedef struct					/* structure containing the bonus fruit counter variable (bon_count) and boolean flag for the presence or absence of bonus fruit in the land. 	*
								 *	[Every ten fruits eaten, if there isn't a previously generated bonus fruit on the land, a bonus fruit will be generated.] 					*/
{
	bool bon;
	int bon_count;
}Fruit;

typedef struct					/* structure containing the variable cmd (in which the input command will be placed, [arrow key]) and the 4 Boolean flags to keep memory of the direction of the snake */
{
	char cmd;
	bool up, down, left, right;
}Command;

typedef struct					/* structure containing the length of the snake and a flexible vector (allocated at run-time) of pointers to character (to the body of the snake) */
{
	int len_snake;
    int score;          		/* score */
    bool dead;         			/* boolean variable for the assertion of the snake's dead */
	char *p[];
}Snake;

typedef struct					/* structure containging the variables relative to the time passed in the game(usefull for framing the game) */
{
	long count1, count2;				/* counter for the seconds */
	float vel, vel1, vel2;				/* temporal distances between the printing of one screen and another[for different rate of the snake] */
	long sec;
	float last_time;					/* variable containing the value (expressed in seconds) to be reached so that a new screen can be printed */
	clock_t start_clock, current_time;
	float diff;
}Timer;


/* function prototypes */

void pause(Timer *timer);

void timer_update(Timer *timer);

void timer_update2(Timer *timer);

void arrow_controller(Command *command, Timer *timer);

void input_controller(Command *command);

void random_fruit(char *campo);

void random_bonus(char *campo);

void eat_fruit(Fruit *fruit, Snake *snake, char *campo);

void eat_bonus_fruit(Fruit *fruit, Snake *snake);

void GoToXY(int colonna, int linea);

void ShowConsoleCursor(bool showFlag);

void intro(char campo[20][70], const char scritta[5][25]);

void move_right(Snake *snake);

void move_left(Snake *snake);

void move_up(Snake *snake);

void move_down(Snake *snake);

void game_over(char campo[ALT][LARG], const char scitta2[ALT_SCRITTA][51]);

void podio(Snake *snake);

void stampa_classifica(int position, Classifica *classifica);



int main(int argc, char *argv[])
{

	/* two-dimensional arrays for initial and final writings */

	const char scritta[5][25] = 	{{z, z, z, z, ' ', z, ' ', ' ', z, ' ', z, z, z, z, ' ', z, ' ', ' ', z, ' ', z, z, z, z, '\0'},
                                    {z, ' ', ' ', ' ', ' ', z, z, ' ', z, ' ', z, ' ', ' ', z, ' ', z, ' ', z, ' ', ' ', z, '\0'},
                                    {z, z, z, z, ' ', z, ' ', z, z, ' ', z, z, z, z, ' ', z, z, ' ', ' ', ' ', z, z, z, z, '\0'},
                                    {' ', ' ', ' ', z, ' ', z, ' ', ' ', z, ' ', z, ' ', ' ', z, ' ', z, ' ', z, ' ', ' ', z, '\0'},
                                    {z, z, z, z, ' ', z, ' ', ' ', z, ' ', z, ' ', ' ', z, ' ', z, ' ', ' ', z, ' ', z, z, z, z, '\0'},};


	const char scritta2[5][51] =  { {z, z, z, z, z, ' ', z, z, z, z, z, ' ', z, ' ', ' ', ' ', z, ' ', z, z, z, z, z, z, ' ', ' ', ' ', z, z, z, z, z, ' ', z, ' ', ' ', ' ', z, ' ', z, z, z, z, z, ' ', z, z, z, z, z, '\0'},
                                    {z, ' ', ' ', ' ', ' ', ' ', z, ' ', ' ', ' ', z, ' ', z, z, ' ', z, z, ' ', z, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', z, ' ', ' ', ' ', z, ' ', z, ' ', ' ', ' ', z, ' ', z, ' ', ' ', ' ', ' ', ' ', z, ' ', ' ', ' ', z, '\0'},
                                    {z, ' ', ' ', z, z, ' ', z, z, z, z, z, ' ', z, ' ', z, ' ', z, ' ', z, z, z, z, z, z, ' ', ' ', ' ', z, ' ', ' ', ' ', z, ' ', ' ', z, ' ', z, ' ', ' ', z, z, z, z, z, ' ', z, z, z, z, ' ', '\0'},
                                    {z, ' ', ' ', ' ', z, ' ', z, ' ', ' ', ' ', z, ' ', z, ' ', ' ', ' ', z, ' ', z, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', z, ' ', ' ', ' ', z, ' ', ' ', z, ' ', z, ' ', ' ', z, ' ', ' ', ' ', ' ', ' ', z, ' ', ' ', ' ', z, '\0'},
                                    {z, z, z, z, z, ' ', z, ' ', ' ', ' ', z, ' ', z, ' ', ' ', ' ', z, ' ', z, z, z, z, z, z, ' ', ' ', ' ', z, z, z, z, z, ' ', ' ', ' ', z, ' ', ' ', ' ', z, z, z, z, z, ' ', z, ' ', ' ', ' ', z, '\0'}};

    /* two-dimensional vector on which the whole lot is based and on which all the operations will be done */

	char campo[ALT][LARG] = {{A_S, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, A_D, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', SNAKE_PIXEL, SNAKE_PIXEL, SNAKE_PIXEL, SNAKE_HEAD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'},
				{B_S, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_O, B_D, '\0'}};


	int i;  /* cicle index */

    /**********************************************
    * structure relative to the fruit			  *
    **********************************************/

	Fruit fruit = {.bon = 0, .bon_count = 0};

	/********************************************
	* structure relative to the commands		*
	********************************************/

	Command command = { .cmd = 'd', .up = 0, .down = 0, .left = 0, .right = 1};		/* initialization of the command structure within the right direction of the snake */

    /**********************************
    * structure relative to the snake *
    **********************************/

	Snake *snake = malloc(sizeof(Snake) + (4 * sizeof(char *)));       /* first dynamic allocation for the snake structure */

	snake->len_snake = 3;												/* initialization with the starting snake parameters */
	snake->score = 0;
	snake->dead = 0;

    snake->p[0] = &campo[10][19];
  	snake->p[1] = &campo[10][18];
    snake->p[2] = &campo[10][17];
    snake->p[3] = &campo[10][16];        								/* initialization of the pointer vector with the right addresses of the land array */

    /********************************************************************
    * structure relative to the timer and the rates o fthe snake		*
    ********************************************************************/

	Timer timer = {.vel = 0.066f, .vel1 = 0.066f, .vel2 = 2 * (0.066f), .last_time = 0.0f};


	ShowConsoleCursor(false);

    srand((unsigned)time(NULL));		/* initialization with a random seed */

    intro(campo, scritta);      		/* game intro */

    time_t *tmppointer = NULL;                          /* initialization of the temporal variables of the timer structure */
    timer.count2 = timer.count1 = time(tmppointer);

    random_fruit(&campo[0][0]);       		/* places the first fruit on the land */

	timer.start_clock = clock();			/* initialize the start_clock variable with the value in seconds of the difference between the moment the program was launched and this instant */


    /******************************************************************************
    * main cicle of the game where you can exit only by losing or by reaching the *
    * max snake lenght														      *
    ******************************************************************************/

    while(snake->dead == 0)
	{

		timer_update(&timer);

		if(timer.diff - timer.last_time >= timer.vel)                  // if the frame in the console has had a life of more than 0.066 s it prints the new frame updating the seconds and the last time variable by adding 0.066
		{                                           				   
			timer_update2(&timer);

			fflush(stdout);

			GoToXY(0, 0);       		/* place the cursor in the top-left angle for print later a screen */

			printf("\n\t\t\ttime: %5d\t\tscore: %d\n\n", timer.sec, snake->score);

			for(i = 0; i < 20; i++)
			{
				printf("\t%s\n", campo[i]);
			}

			if(kbhit() == 0)        // if no key is pressed do nothing otherwise take the pressed character as a command.
			{                       
				;
			}
			else
			{
				arrow_controller(&command, &timer);		  /* take in input only the arrow keys, or the ' ' for pause the game */
                input_controller(&command);       		 /* prevent to change the snake directin in the opposite of the actual direction */
            }


			/*****************************************
			* switch for handling the command input  *
			*****************************************/

			switch(command.cmd)
			{
				case 'a':
                {
                    timer.vel = timer.vel1;                 /* adapts the rate of the snake for the orizzontal movement */
                    command.up = 0;
                    command.down = 0;

                    if(*(snake->p[0] - 1) == ' ')			// if in the position there is the ' ' character
                    {										// refresh the direction flag and move the snake
                        command.left = 1;
                        move_left(snake);
                        break;
                    }
                    else
                    {
                        if(*(snake->p[0] - 1) == FRUIT)									// if in the position there is the FRUIT character
                        {																// refresh the direction flag and move the snake
                            command.left = 1;
                            eat_fruit(&fruit, snake, &campo[0][0]);
                            move_left(snake);
                            break;
                        }
                        else
                        {
                            if(*(snake->p[0] - 1) == BONUS_FRUIT)						// se nella posizione in cui sta andando la testa c'è un carattere BONUS_FRUIT
                            {															// refresh the direction flag, eat the bonus fruit and move the snake
                                command.left = 1;
                                eat_bonus_fruit(&fruit, snake);
                                move_left(snake);
                                break;
                            }
                        }
                    }

                    snake->dead = 1;			/* if none of the previously instances has been verified the snake crashed into the border or he bited itself */
                    break;
                }
                case 'd':
                {
                    timer.vel = timer.vel1;             /* adapts the snake rate to the orizzontal movement */
                    command.up = 0;
                    command.down = 0;

                    if(*(snake->p[0] + 1) == ' ')
                    {
                        command.right = 1;
                        move_right(snake);
                        break;
                    }
                    else
                    {
                        if(*(snake->p[0] + 1) == FRUIT)
                        {
                            command.right = 1;
                            eat_fruit(&fruit, snake, &campo[0][0]);
                            move_right(snake);
                            break;
                        }
                        else
                        {
                            if(*(snake->p[0] + 1) == BONUS_FRUIT)
                            {
                                command.right = 1;
                                eat_bonus_fruit(&fruit, snake);
                                move_right(snake);
                                break;
                            }
                        }
                    }

                    snake->dead = 1;
                    break;
                }
				case 'w':
                {
                    timer.vel = timer.vel2;             /* adapts the snake rate to the vertical movement */
                    command.left = 0;
                    command.right = 0;

                    if(*(snake->p[0] - LARG) == ' ')
                    {
                        command.up = 1;
                        move_up(snake);
                        break;
                    }
                    else
                    {
                        if(*(snake->p[0] - LARG) == FRUIT)
                        {
                            command.up = 1;
                            eat_fruit(&fruit, snake, &campo[0][0]);
                            move_up(snake);
                            break;
                        }
                        else
                        {
                            if(*(snake->p[0] - LARG) == BONUS_FRUIT)
                            {
                                command.up = 1;
                                eat_bonus_fruit(&fruit, snake);
                                move_up(snake);
                                break;
                            }
                        }
                    }

                    snake->dead = 1;
                    break;
                }
				case 's':
                {
                    timer.vel = timer.vel2;                 /* adapts the snake rate to the vertical movement */
                    command.left = 0;
                    command.right = 0;

                    if(*(snake->p[0] + LARG) == ' ')
                    {
                        command.down = 1;
                        move_down(snake);

                        break;
                    }
                    else
                    {
                        if(*(snake->p[0] + LARG) == FRUIT)
                        {
                            command.down = 1;
                            eat_fruit(&fruit, snake, &campo[0][0]);
                            move_down(snake);

                            break;
                        }
                        else
                        {
                            if(*(snake->p[0] + LARG) == BONUS_FRUIT)
                            {
                                command.down = 1;
                                eat_bonus_fruit(&fruit, snake);
                                move_down(snake);
                                break;
                            }
                        }
                    }

                    snake->dead = 1;
                    break;
                }

				default:
                {
                    break;
                }
            }   /* switch end */

        }	/* endif */
    }   /* while end */

    GoToXY(0, 0);						/* place the cursor at the top left and print the game end screen with the ranking (if the score is in the top three) */

    game_over(campo, scritta2);

    podio(snake);

    GoToXY(0, 25);

    i = _getch();					/* wait for the last key pressed to terminate the game */

    return 0;
}               /* end of main */


/*******************************************************
* function for the update of the timer and for pause   *
* the game											   *
*******************************************************/
void pause(Timer *timer)
{
	while(kbhit() == 0 || _getch() != ESC)		/* while no key is pressed do nothing */
	{
		;
	}

	timer_update(timer);				/* refresh the temporal variables */
	timer->last_time = timer->diff;
}

void timer_update(Timer *timer)
{
	timer->current_time = clock();																		// update current_time with the time passed since the beginning of the program to this timme
	timer->diff = ((timer->current_time - timer->start_clock) / (float)(CLOCKS_PER_SEC));				// update diff with the actual time elapsed since the start of the game (expressed in seconds)
}

void timer_update2(Timer *timer)
{
	static time_t *tmppointer;
	timer->count1 = time(tmppointer);					//  update the count1 variable with the value of the seconds elapsed since the beginning of the program
	timer->sec = timer->count1 - timer->count2;			// update the sec variable with the seconds actually passed since the start of the game
    timer->last_time += timer->vel;						// updates the last_time variable with the value (expressed in seconds) to be reached to print a new screen
}

/******************************************************
* function for the input survey						  *
******************************************************/

void arrow_controller(Command *command, Timer *timer)
{
	static int ch;

	ch = _getch();

	if(ch == ' ')								// if ' ' is pressed pause the game
	{
		pause(timer);
	}

	if(ch == ESC || ch == ' ')								// if the key pressed is an arrow key, then the char returned by the _getch() is 0xE0, or in the case nwe were in pause is ' '
	{
		ch = _getch();										// take the second character of the escape sequence and verify wich arrow key has been pressed

		switch(ch)
		{
			case 72: command->cmd = 'w'; break;
			case 80: command->cmd = 's'; break;
			case 75: command->cmd = 'a'; break;
			case 77: command->cmd = 'd'; break;
		}
	}
}

/*********************************************************************************************************
* function for controlling that the snake doesn't follw a direction oppsite to that wich hes going to    *
*********************************************************************************************************/

void input_controller(Command *command)
{
    if(command->cmd == 'a' && command->right == 1)           // prevents the change of direction in the opposite direction to that in which you are going
    {
       command->cmd = 'd';
    }
    else
    {
        if(command->cmd == 'w' && command->down == 1)
        {
            command->cmd = 's';
        }
        else
        {
            if(command->cmd == 'd' && command->left == 1)
            {
                command->cmd = 'a';
            }
            else
            {
                if(command->cmd == 's' && command->up == 1)
                {
                    command->cmd = 'w';
                }

            }
        }
    }
}

/*************************************************************************
* function for handling the moment in wich the snake eat the bonus fruit   *
*************************************************************************/

void eat_bonus_fruit(Fruit *fruit, Snake *snake)
{
    fruit->bon = 0;              				// update to zero the flag of the bonus fruit
    snake->score += 10;         				// increase the score by ten
    (snake->len_snake)++;         				// increase the length of the snake

    snake = realloc(snake, sizeof(Snake) + ((snake->len_snake + 1) * sizeof(char *)));			// allocates the snake structure with the new adapted flexible array member length
    snake->p[snake->len_snake] = NULL;															// initialize the last element of the flexible array member to NULL
}


/*************************************************************************
* function for handling the moment in wich the snake eat the fruit   	 *
*************************************************************************/

void eat_fruit(Fruit *fruit, Snake *snake, char *campo)
{
    (fruit->bon_count)++;               				
    snake->score++;        									
    (snake->len_snake)++;         						

    snake = realloc(snake, sizeof(Snake) + ((snake->len_snake + 1) * sizeof(char *)));
    snake->p[snake->len_snake] = NULL;														

    random_fruit(campo);             						// place a fruit in a random spot on the land

    if(fruit->bon_count % 10 == 0 && fruit->bon == 0)      // if the bonus counter is a ten multiple and there is no bonus fruit on the land
    {
        random_bonus(campo);         				// place a bonus fruit in a random position on the land
        fruit->bon = 1;						        // update to 1 the bonus fruit flag
    }
}

/***************************************************
* function for the handling of the fruit printing  *
***************************************************/

void random_fruit(char *campo)
{
	static int fruitpos;
	static char *change;

	while(1)                                /* while we can't find  good position for the fruit retry to find a good position */
	{
	    change = campo;

		fruitpos = rand() % 1400;
		change += fruitpos;

		if(*change == ' ')
		{
			*change = FRUIT;
			break;
		}
	}
}

/************************************************
* function for print the fruit					*
************************************************/

void random_bonus(char *campo)
{
	static int bonuspos;               /* static integer for the position fo the bonus fruit on the land */
    static char *change;

	while(1)                          			
	{
	    change = campo;                          
		bonuspos = rand() % 1400;
		change += bonuspos;

		if(*change == ' ')
		{
			*change = BONUS_FRUIT;
			break;
		}
	}
}

/***************************************************************
* function for moving the snake by one position to the right   *
***************************************************************/

void move_right(Snake *snake)
{
    static char *change;
    static int i;

    *(snake->p[0]) = SNAKE_PIXEL;            	  //  instead of the head, place a SNAKE_PIXEL char
    change = snake->p[0];                    	  //	 save the address of the head in the change variable
    snake->p[0]++;                           	  //	 increase by a unit the address of the  head
    *(snake->p[0]) = SNAKE_HEAD;             	  //  place in the new position of the head the SNAKE_HEAD char

    if(snake->p[snake->len_snake] != NULL)       // if a fruit isn't been eated p[snake->len_snake] != NULL
	{
		*(snake->p[snake->len_snake]) = ' ';	 // then place a space character in the old tail position
	}

    for(i = snake->len_snake; i > 1; i--)  		// from the tail to the position before the neck it exchanges the current address with that of the lower part in the address vector of the snake
    {                               			
        snake->p[i] = snake->p[i - 1];
    }
	snake->p[1] = change;						// finally he places the address that the head previously possessed in the current address of the neck
}

/***************************************************************
* function for moving the snake by one position to the left    *
***************************************************************/

void move_left(Snake *snake)
{
    static char *change;
    static int i;

    *(snake->p[0]) = SNAKE_PIXEL;
    change = snake->p[0];
    snake->p[0]--;
    *(snake->p[0]) = SNAKE_HEAD;

    if(snake->p[snake->len_snake] != NULL)
	{
		*(snake->p[snake->len_snake]) = ' ';
	}

    for(i = snake->len_snake; i > 1; i--)
    {
        snake->p[i] = snake->p[i - 1];
    }
    snake->p[1] = change;
}

/***************************************************************
* function for moving the snake by one position up			   *
***************************************************************/

void move_up(Snake *snake)
{
    static char *change;
    static int i;

    *(snake->p[0]) = SNAKE_PIXEL;
    change = snake->p[0];
    snake->p[0] -= LARG;
    *(snake->p[0]) = SNAKE_HEAD;

 	if(snake->p[snake->len_snake] != NULL)
	{
		*(snake->p[snake->len_snake]) = ' ';
	}

    for(i = snake->len_snake; i > 1; i--)
    {
        snake->p[i] = snake->p[i - 1];
    }
    snake->p[1] = change;
}

/***************************************************************
* function for moving the snake by one position down		   *
***************************************************************/

void move_down(Snake *snake)
{
    static char *change;
    static int i;

    *(snake->p[0]) = SNAKE_PIXEL;
    change = snake->p[0];

    snake->p[0] += LARG;
    *(snake->p[0]) = SNAKE_HEAD;

	if(snake->p[snake->len_snake] != NULL)
	{
		*(snake->p[snake->len_snake]) = ' ';
	}

    for(i = snake->len_snake; i > 1; i--)
    {
        snake->p[i] = snake->p[i - 1];
    }
    snake->p[1] = change;
}


/***************************************************************
* function for moving the cursor							   *
***************************************************************/

void GoToXY(int colonna, int linea)
{
    static COORD coord;        // declaration of a COORD structure
    coord.X = colonna;  	   // cursor coordinates
    coord.Y = linea;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);   // save in the hConsole variable(type void *) the value returned by the function(the handle of the screen)

    if (!SetConsoleCursorPosition(hConsole, coord))      // function call that allowed the cursor movement
    {
        ;												// if the function fail do nothing
    }
}

/***************************************************************
* functino for maknig the cursor not visible				   *
***************************************************************/

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; 			// set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}



/***************************************************************
* function for handlig the end game screen					   *
***************************************************************/

void game_over(char campo[ALT][LARG],const char scritta2[ALT_SCRITTA][51])
{
    static int i;      /* indice dei cicli */

    static char linea[LARG] = 	{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'};

    printf("\n\n\n\t%s\n", campo[0]);           /* print the background */

    for(i = 1; i < (ALT - 1); i++)
    {
        printf("\t%s\n", linea);
    }

    printf("\t%s\n\n", campo[ALT - 1]);


    for(i = 0; i < ALT_SCRITTA; i++)                /* print the writing */
    {
        GoToXY(18, i + 6);
        puts(scritta2[i]);
    }

    i = getch();               /* wait the input */
}


/***************************************************************
* function for handling the start screen 					   *
***************************************************************/

void intro(char campo[ALT][LARG], const char scritta[ALT_SCRITTA][25])
{
    static int i;      							/* index of cicle */

    static const char linea[LARG] = 	{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'};

	printf("\n\n\n\t%s\n", campo[0]);			// print the background

    for(i = 1; i < (ALT - 1); i++)
    {
        printf("\t%s\n", linea);
    }

    printf("\t%s\n\n", campo[ALT - 1]);			// print the writing

    for(i = 0; i < ALT_SCRITTA; i++)
    {
        GoToXY(30, i + 6);
        puts(scritta[i]);
    }

    GoToXY(18, 14);

    printf("\tusare le freccie per muoversi...");

    GoToXY(18, 16);

    printf("\tpress the spacebar for pause the game...");

    GoToXY(18, 18);

    printf("PRESS EVERY KEY FOR START...");

    i = getch();													// wait an input
}

/****************************************************
* function for handlig the ending screen            *
****************************************************/

void podio(Snake *snake)
{
    FILE *fp;      										 	 /* pointer to the ranking file */

    Classifica classifica = {.name_player = '\0'};      	/* declaration of a ranking structure */

    fp = fopen(CLASSIFICA, "r+");       					/* open the ranking file in read/write mode */

    if(fp == NULL)                                          // if the file doesn't exist print an error message */
    {
        printf("error: the file can't opened.\n");

    }
    else                                                    // otherwise update the rnaking file if necessary
    {
        fscanf(fp, "%s\t%d\n%s\t%d\n%s\t%d\n", &classifica.name1, &classifica.score1, &classifica.name2, &classifica.score2, &classifica.name3, &classifica.score3);        /* read th data of the current rank */
        fflush(fp);

	    if(snake->score > classifica.score3 && snake->score < classifica.score2)      /* if the player reached the third position */
	    {
	        classifica.score3 = snake->score;
	        fseek(fp, 0L, SEEK_SET);
	        stampa_classifica(3, &classifica);
	        fprintf(fp, "%3s\t%d\n%3s\t%d\n%3s\t%d\n", classifica.name1, classifica.score1, classifica.name2, classifica.score2, classifica.name_player, snake->score);
	        fclose(fp);
	    }
	    else
	    {
	        if(snake->score > classifica.score2 && snake->score < classifica.score1)      /* if the player reach the second position */
	        {
	            classifica.score2 = snake->score;
	            fseek(fp, 0L, SEEK_SET);
	            stampa_classifica(2, &classifica);
	            fprintf(fp, "%3s\t%d\n%3s\t%d\n%3s\t%d\n", classifica.name1, classifica.score1, classifica.name_player, snake->score, classifica.name3, classifica.score3);
	            fclose(fp);
	        }
	        else
	        {
	            if(snake->score > classifica.score1)           								/* if the player reach the first position */
	            {
	                classifica.score1 = snake->score;
	                fseek(fp, 0L, SEEK_SET);
	                stampa_classifica(1, &classifica);
	                fprintf(fp, "%3s\t%d\n%3s\t%d\n%3s\t%d\n", classifica.name_player, snake->score, classifica.name2, classifica.score2, classifica.name3, classifica.score3);
	                fclose(fp);
	            }
	        }
	    }
	}
}

/****************************************
* function for rank printing            *
****************************************/

void stampa_classifica(int position, Classifica *classifica)
{
    int i;

    switch(position)
    {
        case(1):                        /* print the rank with the new first position name */
        {
            GoToXY(34, 14);
            printf("        %d", classifica->score1);
            GoToXY(34, 16);
            printf("%s     %d", classifica->name2, classifica->score2);
            GoToXY(34, 18);
            printf("%s     %d", classifica->name3, classifica->score3);
            GoToXY(34, 14);

            for(i = 0; i < 3; i++)
            {
                classifica->name_player[i] = getch();
                printf("%c", classifica->name_player[i]);
            }

            break;
        }

        case(2):                       /* print the rank with the new second position name */
        {
            GoToXY(34, 14);
            printf("%s     %d", classifica->name1, classifica->score1);
            GoToXY(34, 16);
            printf("        %d", classifica->score2);
            GoToXY(34, 18);
            printf("%s     %d", classifica->name3, classifica->score3);
            GoToXY(34, 16);

            for(i = 0; i < 3; i++)
            {
                classifica->name_player[i] = getch();
                printf("%c", classifica->name_player[i]);
            }

            break;
        }

        case(3):                    /* print the rank with the new third position name */
        {
            GoToXY(34, 14);
            printf("%s     %d", classifica->name1, classifica->score1);
            GoToXY(34, 16);
            printf("%s     %d", classifica->name2, classifica->score2);
            GoToXY(34, 18);
            printf("        %d", classifica->score3);
            GoToXY(34, 18);

            for(i = 0; i < 3; i++)
            {
                classifica->name_player[i] = getch();
                printf("%c", classifica->name_player[i]);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}
