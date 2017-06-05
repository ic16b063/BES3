/*
 * @file my_empfaenger
 * Betriebssysteme Sender Empfänger
 * Beispiel 3
 *
 * @author Karl Heinz Steinmetz, Ieriel Stanescu, Christian Posch
 * @date 2017/06/05
 *
 * @version 1.0.0
 *
 * @todo
 */

/*
 * -------------------------------------------------------------- includes --
 */

 /* include des eigenen headerfiles */
  #include "grp_0-shared.h"

  #include <unistd.h>
  #include <stdio.h>
  #include <sys/types.h>
  #include <getopt.h>
 // #include <sys/wait.h>
 // #include <errno.h>
  #include <string.h>
  #include <stdlib.h>
#include <limits.h>
  
  //#include <signal.h>
 
 /*
 * --------------------------------------------------------------- defines --
 */
 
 
/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------------------- globals --
 */
	// static int child_pid;
	// static FILE *given_fp;
/*
 * ------------------------------------------------------------- functions --
 */
 
 
int main (int argc, char * const argv[])
{
	// Einlesen der Parameter mittels getopt
	
	int opt;
	char *endptr;
	int puffer = 0; // Größe (Anzahl an Feldern) des Ringpuffers
	int testmodus = 0;
	int only_clean = 0;
	

	opt=0;
	// mit Schleife alle Parameter durchgehen bis m gefunden wurde
	while ((opt = getopt(argc, argv, "m:,t,c")) != -1)
	{
		if( opt == 'm')
		{
			printf("errorno: %d\noptarg: %s\n",errno, optarg);
			errno = 0;
			puffer = strtol(optarg, &endptr, 10);
			if((errno == ERANGE && (puffer == LONG_MAX || puffer == LONG_MIN)) || (errno != 0 && puffer == 0))
			{
				//fprintf(stderr,"%s: %s\n", glob_var, strerror(errno));
				printf("Convertfehler bei strotol, Nr: %d, Text: %s\n", errno, strerror(errno));
				//printf("Rangefehler bei strotol\n");
				return -1; //exit(EXIT_FAILURE);
			}
		}
		else if (opt == 't')
		{
			if (only_clean == 0)
			testmodus = 1;
			//printf("Testmodus\n\n");
		}
		else if (opt == 'c')
		{
			if (testmodus == 0)
			only_clean = 1;
		}
		
		else
		{
			errno = EINVAL;
			fprintf(stderr, "Fehler: %s\n", strerror(errno));
			//printf("Optionsfehler??\n");
			//return -1; //exit(EXIT_FAILURE);
		}
		
		
		
	}
	
	//printf("Puffergröße soll %d sein.\n",puffer);
	
	printf("Funktionstest %s\n", argv[0]);
	
	int my_uid = getuid();
	
	int my_shm=0;
	int my_sem1=0;
	int my_sem2=0;
	
	if (only_clean==0)
	{
		// erstelle shared memmory
		my_shm = create_shm(my_uid, puffer);
		// erstelle Semaphore 0 (Sender)
		my_sem1 = create_sem(my_uid, 1);
		// erstelle Semaphore 1 (Empfänger)
		my_sem2 = create_sem(my_uid, 0);

		printf("UID: %d, Shared Memory ID: %d\n", my_uid, my_shm);
		printf("UID: %d, Semaphore 1 ID: %d\n", my_uid, my_sem1);
		printf("UID: %d, Semaphore 2 ID: %d\n", my_uid, my_sem2);		
	}
	
		
	if (testmodus != 1)	printf("Clean sucess: %d\n", clean_all(my_shm, my_sem1, my_sem2));
	else printf("TESTMODUS, no cleaning\n");

	return 1;

	

}	
 

