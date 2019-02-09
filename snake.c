/* PROGETTO ESAME DI PROGRAMMAZIONE */

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>

#define ALT 20					/* altezza del campo */
#define LARG 70					/* larghezza del campo */

#define B_V (char)186			/* carattere per il bordo verticale */
#define B_O (char)205			/* carattere per il bordo orizzontale */
#define z 	(char)219			/* carattere per le scritte */
#define A_S (char)201			/* carattere per l'angolo in alto a sinistra */
#define A_D (char)187			/* carattere per l'angolo in alto a destra */
#define B_S (char)200			/* carattere per il bordo sinistro */
#define B_D (char)188			/* carattere per il bordo destro */

#define SNAKE_PIXEL (char)219         /* carattere per il corpo dello snake */
#define SNAKE_HEAD (char)177          /* carattere per la testa dello snake */
#define ESC 0xE0			  		/* valore esadecimale di escape che ritornato dalla funzione _getch() garantisce che è stata premuta una freccia nella tastiera */

#define FRUIT (char)111               /* carattere per rappresentare la frutta */
#define BONUS_FRUIT (char)169         /* carattere per rappresentare la frutta bonus */

#define ALT_SCRITTA 5               /* altezza delle scritte */

#define CLASSIFICA "classifica.txt"     /* nome del file testuale contenente i valori della classifica di gioco */


typedef struct           		/* struttura contenente nomi e punteggi dei migliori 3 */
{
    char name_player[4];
    char name1[4], name2[4], name3[4];
    int score1, score2, score3;
}Classifica;

typedef struct					/* struttura contenente le variabili contatore della frutta bonus(bon_count) e flag booleano per la presenza o meno della frutta bonus nel campo */
{
	bool bon;
	int bon_count;
}Fruit;

typedef struct					/* struttura contenente la variabile cmd (nella quale si porrà il comando in input) ed i 4 flag booleani per tenere memoria della direzione dello snake */
{
	char cmd;
	bool up, down, left, right;
}Command;

typedef struct					/* struttura contenente la lunghezza dello snake ed un vettore flessibile(allocato a run-time) di puntatori a carattere (al corpo delllo snake) */
{
	int len_snake;
    int score;          		/* punteggio */
    bool dead;         			/* variabile booleana per asserire la morte dello snake */
	char *p[];
}Snake;

typedef struct					/* struttura contenente le variabili relative al tempo trascorso durante la partita */
{
	long count1, count2;				/* contatori per i secondi */
	float vel, vel1, vel2;				/* distanze temporali tra la stampa di una schermata ed un'altra */
	long sec;
	float last_time;					/* variabile contenente il valore(espresso in secondi) da raggiungere affinche si possa stampare una nuova schermata */
	clock_t start_clock, current_time;
	float diff;
}Timer;


/* prototipi delle varie funzioni */

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

	/* vettori bidimensionali per le scritte di intro e fine gioco */

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

    /* vettore bidimensionale su cui si basa l'intera partita e su cui si faranno tutte le operazioni */

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


	int i;  /* indice dei cicli */

    /**********************************************
    * struttura relativa alla frutta e ai comandi *
    **********************************************/

	Fruit fruit = {.bon = 0, .bon_count = 0};

	/********************************************
	* struttura relativa ai comandi in input	*
	********************************************/

	Command command = { .cmd = 'd', .up = 0, .down = 0, .left = 0, .right = 1};		/* inizializza la struttura con il comando ed i flag per il movimento verso destra */

    /********************************
    * struttura relativa allo snake *
    ********************************/

	Snake *snake = malloc(sizeof(Snake) + (4 * sizeof(char *)));       /* puntatore alla struttura di tipo Snake la cui dimensione è allocata dinamicamente a run_time */

	snake->len_snake = 3;												/* inizializzazione della struttura snake */
	snake->score = 0;
	snake->dead = 0;

    snake->p[0] = &campo[10][19];
  	snake->p[1] = &campo[10][18];
    snake->p[2] = &campo[10][17];
    snake->p[3] = &campo[10][16];        								/* inizializza il vettore di puntatori al corpo dello snake */

    /********************************************************************
    * struttura relativa al timer di gioco ed alle velocità dello snake *
    ********************************************************************/

	Timer timer = {.vel = 0.066f, .vel1 = 0.066f, .vel2 = 2 * (0.066f), .last_time = 0.0f};


    /****************************************************************************************
    * fine della parte dichiarativa del programma ed inizio della parte di inizializzazione *
	****************************************************************************************/

	ShowConsoleCursor(false);

    srand((unsigned)time(NULL));		/* inizializzatore random per la posizione della frutta */

    intro(campo, scritta);      		/* entrata nel menu di gioco */

    time_t *tmppointer = NULL;                          /* inizializzazione delle vaiabili temporali */
    timer.count2 = timer.count1 = time(tmppointer);

    random_fruit(&campo[0][0]);       		/* pone il primo frutto frutto in posizione random sullo schermo */

	timer.start_clock = clock();			/* inizializza la variabile start_clock con il valore in secondi della differenza tra l'attimo in cui il programma è stato lanciato e questo istante */


    /******************************************************************************
    * ciclo principale del gioco dal quale si esce solamente perdendo o arrivando *
    * ad avere la lunghezza massima dello snake, asserendo la variabile dead.     *
    ******************************************************************************/

    while(snake->dead == 0)
	{

		timer_update(&timer);

		if(timer.diff - timer.last_time >= timer.vel)                  // se il frame presente nella console ha avuto una vita superiore a 0,066 s
		{                                           				   // stampa il nuovo frame aggiornando i secondi e la variabile lasttime aagiungendogli 0,066.
			timer_update2(&timer);

			fflush(stdout);

			GoToXY(0, 0);       		/* posiziona il cursore nell'angolo in alto a sinistra della console e ristampa la schermata nuova */

			printf("\n\t\t\ttime: %5d\t\tscore: %d\n\n", timer.sec, snake->score);

			for(i = 0; i < 20; i++)
			{
				printf("\t%s\n", campo[i]);
			}

			if(kbhit() == 0)        // se non viene premuto alcun tasto non fare nulla
			{                       // altrimenti prendi come comando il carattere premuto.
				;
			}
			else
			{
				arrow_controller(&command, &timer);		  /* prende in input solamente i caratteri dei tasti freccia oppure ' ' per mettere in pausa */
                input_controller(&command);       		 /* previene il cambio di direzione in direzione opposta a quella in cui si sta andando */
            }


			/*****************************************
			* switch per gestire il comando in input *
			*****************************************/

			switch(command.cmd)
			{
				case 'a':
                {
                    timer.vel = timer.vel1;                 /* adatta la velocità al movimento orizzontale */
                    command.up = 0;
                    command.down = 0;

                    if(*(snake->p[0] - 1) == ' ')			// se nella posizione in cui sta andando la testa c'è un carattere ' '
                    {										// aggiorna il flag di direzoine e sposta lo snake
                        command.left = 1;
                        move_left(snake);
                        break;
                    }
                    else
                    {
                        if(*(snake->p[0] - 1) == FRUIT)									// se nella posizione in cui sta andando la testa c'è un carattere FRUIT
                        {																// aggiorna il flag di direzoine, mangia la frutta e sposta lo snake
                            command.left = 1;
                            eat_fruit(&fruit, snake, &campo[0][0]);
                            move_left(snake);
                            break;
                        }
                        else
                        {
                            if(*(snake->p[0] - 1) == BONUS_FRUIT)						// se nella posizione in cui sta andando la testa c'è un carattere BONUS_FRUIT
                            {															// aggiorna il flag di direzoine, mangia la frutta bonus e sposta lo snake
                                command.left = 1;
                                eat_bonus_fruit(&fruit, snake);
                                move_left(snake);
                                break;
                            }
                        }
                    }

                    snake->dead = 1;			/*se nessuna delle istanze precedenti è stata verificata allora lo snake è andato contro un bordo oppure si è morso da solo */
                    break;
                }
                case 'd':
                {
                    timer.vel = timer.vel1;             /* adatta la velocità al movimento orizzontale */
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
                    timer.vel = timer.vel2;             /* adatta la velocità ad un movimento verticale */
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
                    timer.vel = timer.vel2;                 /* adatta la velocità ad un movimento verticale */
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

    GoToXY(0, 0);						/* posiziona il cursore in alto a destra e stampa la schermata di fine gioco con la classifica(nel caso il punteggio risulti tra i primi tre) */

    game_over(campo, scritta2);

    podio(snake);

    GoToXY(0, 25);

    i = _getch();					/* attende un input qualsiasi prima di terminare il programma */

    return 0;
}               /* end of main */


/******************************************************
* funzioni per l'aggiornamento delle variabili tempo  *
* nella struttura timer e per mettere in pausa		  *
******************************************************/
void pause(Timer *timer)
{
	while(kbhit() == 0 || _getch() != ESC)		/* finche non viene premuto alcun tasto non fa nulla */
	{
		;
	}

	timer_update(timer);				/* aggiorna le variabili temporali */
	timer->last_time = timer->diff;
}

void timer_update(Timer *timer)
{
	timer->current_time = clock();																		// aggiorna current_time con il tempo trascorso dall'inizio del programma a questo punto
	timer->diff = ((timer->current_time - timer->start_clock) / (float)(CLOCKS_PER_SEC));				// aagiorna diff con il tempo effetivamente trascorso dall'inizio della partita(espresso in secondi)
}

void timer_update2(Timer *timer)
{
	static time_t *tmppointer;
	timer->count1 = time(tmppointer);					//  aggiorna la variabile count1 con il valore dei secondi trascorsi dall'inizio del programma
	timer->sec = timer->count1 - timer->count2;			// aggiorna la variabile sec con i secondi effettivamente trascorsi dall'inizio della partita
    timer->last_time += timer->vel;						// aggiorna la variabile last_time con il valore(espresso in secondi) da raggiungere per stampare una nuova schermata
}

/******************************************************
* funzione per il rilevamento dei caratteri freccia   *
* in input.											  *
******************************************************/

void arrow_controller(Command *command, Timer *timer)
{
	static int ch;

	ch = _getch();

	if(ch == ' ')								// se non si è premuta una freccia ma si è premuto ' ' metti il gioco in pausa
	{
		pause(timer);
	}

	if(ch == ESC || ch == ' ')								// se il carattere premuto è una freccia allora il carattere ritornato dalla _getch() è 0xE0, o nel caso si era in pausa il carattere è ' '
	{
		ch = _getch();										// prende il secondo carattere della sequenza di escape per verificare quale freccia è stata premuta

		switch(ch)
		{
			case 72: command->cmd = 'w'; break;
			case 80: command->cmd = 's'; break;
			case 75: command->cmd = 'a'; break;
			case 77: command->cmd = 'd'; break;
		}
	}
}

/********************************************
* funzione per il controllo dell'input      *
* nel caso si provi a muoversi in direzione *
* opposta a quella in cui si sta andando    *
********************************************/

void input_controller(Command *command)
{
    if(command->cmd == 'a' && command->right == 1)           // previene il cambio di direzione in direzione opposta a quella in cui si sta andando
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

/*******************************************
* funzione per la gestione del momento     *
* in cui lo snake mangia la frutta bonus   *
*******************************************/

void eat_bonus_fruit(Fruit *fruit, Snake *snake)
{
    fruit->bon = 0;              		// azzera il flag della presenza della frutta bonus
    snake->score += 10;         			// incrementa di dieci i punti
    (snake->len_snake)++;         		// incrementa la lunghezza dello snake

    snake = realloc(snake, sizeof(Snake) + ((snake->len_snake + 1) * sizeof(char *)));			// alloca spazio per una struttura Snake ed il suo vettore flessibile
    snake->p[snake->len_snake] = NULL;															// inizializza l'ultimo elemento del vettore flessibile a NULL
}

/*******************************************
* funzione per la gestione del momento     *
* in cui lo snake mangia la frutta normale *
*******************************************/

void eat_fruit(Fruit *fruit, Snake *snake, char *campo)
{
    (fruit->bon_count)++;               				// azzera il flag della presenza della frutta bonus
    snake->score++;        									// incrementa di dieci i punti
    (snake->len_snake)++;         						// incrementa la lunghezza dello snake

    snake = realloc(snake, sizeof(Snake) + ((snake->len_snake + 1) * sizeof(char *)));
    snake->p[snake->len_snake] = NULL;														// inizializza l'ultimo elemento del vettore flessibile a NULL

    random_fruit(campo);             						// aggiunge un frutto in posizione random nello schermo

    if(fruit->bon_count % 10 == 0 && fruit->bon == 0)      // se il contatore bonus è un multiplo di 10 e sul campo non c'è frutta bonus
    {
        random_bonus(campo);         				// aggiungi un frutto bonus in posizione random
        fruit->bon = 1;						        // asserisci il flag della presenza del frutto bonus
    }
}

/***********************************************
* funzione per la stampa a video della frutta  *
***********************************************/

void random_fruit(char *campo)
{
	static int fruitpos;
	static char *change;

	while(1)                                /* finche non si trova una posizione libera per la frutta riprova ed infine riponila in campo[][] */
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
* funzione per stampare a video la frutta bonus *
* ogni 10 frutti normali mangiati.              *
************************************************/

void random_bonus(char *campo)
{
	static int bonuspos;               /* variabile per contenere un valore random per la generazione della frutta bounus */
    static char *change;

	while(1)                          			// finche non trova un punto dello schermo di gioco
	{
	    change = campo;                          // dove si ha uno spazio bianco riprova un 'altro numero random
		bonuspos = rand() % 1400;
		change += bonuspos;

		if(*change == ' ')
		{
			*change = BONUS_FRUIT;
			break;
		}
	}
}

/***************************************
* funzione per muovere lo snake di una *
* posizione verso destra               *
***************************************/

void move_right(Snake *snake)
{
    static char *change;
    static int i;

    *(snake->p[0]) = SNAKE_PIXEL;            	  //  al posto della testa pone un carattere SNAKE_PIXEL
    change = snake->p[0];                    	  //	 salva l'indirizzo della testa nella variabile change
    snake->p[0]++;                           	  //	 aumenta di un'unità il puntatore alla testa
    *(snake->p[0]) = SNAKE_HEAD;             	  //  pone nella nuova posizione della testa il carattere SNAKE_HEAD

    if(snake->p[snake->len_snake] != NULL)       // se non si è mangiata la frutta p[snake->len_snake] != NULL
	{
		*(snake->p[snake->len_snake]) = ' ';	 // allora pone un carattere spazio al posto della coda
	}

    for(i = snake->len_snake; i > 1; i--)  		// dalla coda alla posizione prima del collo scambia l'indirizzo
    {                               			// corrente nel ciclo con quello di un'unità inferiore nel vettore degli indirizzi dello snake
        snake->p[i] = snake->p[i - 1];
    }
	snake->p[1] = change;						// infine pone l'indirizzo che possedeva precedentemente la testa nell'indirizzo attuale del collo
}

/***************************************
* funzione per muovere lo snake di una *
* posizione verso sinistra             *
***************************************/

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

/***************************************
* funzione per muovere lo snake di una *
* posizione verso l'alto               *
***************************************/

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

/***************************************
* funzione per muovere lo snake di una *
* posizione verso il basso             *
***************************************/

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


/*******************************************
* funzione per lo spostamento del cursore  *
* nella console.                           *
*******************************************/

void GoToXY(int colonna, int linea)
{
    static COORD coord;        // dichiarazione di una struttura di tipo COORD
    coord.X = colonna;  	   // nella quale sono specificate le posizioni delle coordinate del cursore
    coord.Y = linea;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);   // salva nella variabile hConsole (di tipo void *) il valore ritornato dalla funzione(l'handle dello schermo) il cui parametro è di tipo long

    if (!SetConsoleCursorPosition(hConsole, coord))      // chiamata alla funzione che permette lo spostamento del cursore
    {
        ;												//se la funzione fallisce non fa nulla
    }
}

/***************************************************************
* funzione per fare diventare il cursore invisibile	o visibile *
***************************************************************/

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}



/**********************************************
* funzione per la gestione della schermata di *
* fine gioco.                                 *
**********************************************/

void game_over(char campo[ALT][LARG],const char scritta2[ALT_SCRITTA][51])
{
    static int i;      /* indice dei cicli */

    static char linea[LARG] = 	{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'};

    printf("\n\n\n\t%s\n", campo[0]);           /* stampa lo sfondo */

    for(i = 1; i < (ALT - 1); i++)
    {
        printf("\t%s\n", linea);
    }

    printf("\t%s\n\n", campo[ALT - 1]);


    for(i = 0; i < ALT_SCRITTA; i++)                /* stampa la scritta */
    {
        GoToXY(18, i + 6);
        puts(scritta2[i]);
    }

    i = getch();               /* aspetta un input */
}


/********************************************
* funzione per la gestione della schermata  *
* iniziale del gioco.                       *
********************************************/

void intro(char campo[ALT][LARG], const char scritta[ALT_SCRITTA][25])
{
    static int i;      /* indice dei cicli */

    static const char linea[LARG] = 	{B_V, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', B_V, '\0'};

	printf("\n\n\n\t%s\n", campo[0]);			// stampa lo sfondo

    for(i = 1; i < (ALT - 1); i++)
    {
        printf("\t%s\n", linea);
    }

    printf("\t%s\n\n", campo[ALT - 1]);			// stampa la scritta

    for(i = 0; i < ALT_SCRITTA; i++)
    {
        GoToXY(30, i + 6);
        puts(scritta[i]);
    }

    GoToXY(18, 14);

    printf("\tusare le freccie per muoversi...");

    GoToXY(18, 16);

    printf("\tpremere spazio per mettere in pausa...");

    GoToXY(18, 18);

    printf("PREMERE UN TASTO QUALSIASI PER CONTINUARE...");

    i = getch();													// attende un input
}

/****************************************************
* funzione per la stampa a video e l'aggiornamento  *
* della classififca di gioco tenuta in memoria      *
* secondaria sottoforma di file di testo.           *
****************************************************/

void podio(Snake *snake)
{
    FILE *fp;        /* puntatore al file testuale classifica */

    Classifica classifica = {.name_player = '\0'};      /* dichiarazione di una strutura classifica */

    fp = fopen(CLASSIFICA, "r+");       /* apre il file di testo classifica nella directory corrente in modalità di lettura-scrittura */

    if(fp == NULL)                                          // se il file non esiste o non si può aprire stampa un messaggio di errore */
    {
        printf("errore: il file non si può aprire.\n");

    }
    else                                                    // altrimenti aggiorna la classifica se necessario
    {
        fscanf(fp, "%s\t%d\n%s\t%d\n%s\t%d\n", &classifica.name1, &classifica.score1, &classifica.name2, &classifica.score2, &classifica.name3, &classifica.score3);        /* legge i dati correnti della classifica */
        fflush(fp);

	    if(snake->score > classifica.score3 && snake->score < classifica.score2)      /* se il giocatore è arrivato terzo aggiorna e stampa la classifica */
	    {
	        classifica.score3 = snake->score;
	        fseek(fp, 0L, SEEK_SET);
	        stampa_classifica(3, &classifica);
	        fprintf(fp, "%3s\t%d\n%3s\t%d\n%3s\t%d\n", classifica.name1, classifica.score1, classifica.name2, classifica.score2, classifica.name_player, snake->score);
	        fclose(fp);
	    }
	    else
	    {
	        if(snake->score > classifica.score2 && snake->score < classifica.score1)      /* se il giocatore è arrivato secondo aggiorna e stampa la classifica */
	        {
	            classifica.score2 = snake->score;
	            fseek(fp, 0L, SEEK_SET);
	            stampa_classifica(2, &classifica);
	            fprintf(fp, "%3s\t%d\n%3s\t%d\n%3s\t%d\n", classifica.name1, classifica.score1, classifica.name_player, snake->score, classifica.name3, classifica.score3);
	            fclose(fp);
	        }
	        else
	        {
	            if(snake->score > classifica.score1)           /* se il giocatore è arrivato primo aggiorna e stampa la classifica */
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
* funzione per la stampa a video della  *
* classifica.                           *
****************************************/

void stampa_classifica(int position, Classifica *classifica)
{
    int i;

    switch(position)
    {
        case(1):                        /* stampa la classifica con l'inserimento del nome al primo posto */
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

        case(2):                       /* stampa la classifica con l'inserimento del nome al secondo posto */
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

        case(3):                    /* stampa la classifica con l'inserimento del nome al terzo posto */
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
