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
 
  #include <string.h>
  #include <stdlib.h>
  #include <limits.h>
  
  #include <error.h>
  #include <errno.h>

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
	// allgemeine Steuervariablen
	
	int testmodus = 0; // zeigt an ob wir uns im Testmodus befinden
	int only_clean = 0; // zeigt an ob nur aufgräumt werden soll


	// Einlesen der Parameter mittels getopt	
	int opt = 0; // speichert den Charakter der Option (nur ein Zeichen möglich)
	char *endptr; // zeigt beim strtol auf den Rest des Strings
	int puffer = 0; // Größe (Anzahl an Feldern) des Ringpuffers
	
	// mit Schleife alle Parameter durchgehen bis m gefunden wurde
	while ((opt = getopt(argc, argv, "m:tc")) != -1)
	{
		switch (opt)
		{
			case 'm':
				errno = 0;
				puffer = strtol(optarg, &endptr, 10);
				if((errno == ERANGE && (puffer == LONG_MAX || puffer == LONG_MIN)) || (errno != 0 && puffer == 0))
				{
					//fprintf(stderr,"%s: %s\n", glob_var, strerror(errno));
					//printf("Convertfehler bei strotol, Nr: %d, Text: %s\n", errno, strerror(errno));
					error_at_line(0, errno,__FILE__,__LINE__,"Error at Range: %s", strerror(errno));
					//printf("Rangefehler bei strotol\n");
					clean_all_auto();
					return -1; //exit(EXIT_FAILURE);
				}
				break;
			case 't':
				if (only_clean == 0) testmodus = 1;
				break;
			case 'c':
				if (testmodus == 0)	only_clean = 1;
				break;
			
			default: /* ? */
				//errno = EINVAL;
				if (opt == '?') fprintf(stderr, "%s : Unknown option \"?\" encountered!\n", argv[0]); //strerror(errno));
				fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]); //strerror(errno));
				//error_at_line(0, errno,__FILE__,__LINE__,"%s", strerror(errno));
				clean_all_auto();
				return -1;
				//exit(EXIT_FAILURE);
		}
	}
		
	//printf("Puffergröße soll %d sein.\n",puffer);
	
	//printf("Funktionstest %s\n", argv[0]);

	
	
	
	int my_shm=0;
	int my_sem0=0;
	int my_sem1=0;
	
	if (only_clean==0)
	{
		// erstelle shared memmory
		my_shm = create_shm(puffer);
		// erstelle Semaphore 0 (Sender)
		my_sem0 = create_sem0(0);
		// erstelle Semaphore 1 (Empfänger)
		my_sem1 = create_sem1(puffer);
		
		// int my_uid = getuid();
		// printf("UID: %d, Shared Memory ID: %d\n", my_uid, my_shm);
		// printf("UID: %d, Semaphore 0 ID: %d\n", my_uid, my_sem0);
		// printf("UID: %d, Semaphore 1 ID: %d\n", my_uid, my_sem1);		
	
		
	
	
		/*Beginne ab hier mit dem eigentlichen Programm*/
		int *shmptr = shmat(my_shm,NULL,SHM_RDONLY);
		if (shmptr == (int*) -1)
		{
			printf("Attach war nicht erfolgreich.");
			clean_all_auto();
			return -1;
		}
		
		 // printf("Versuche Semaphoren Werte auszugeben:\n");
		
		 // printf("Semaphore lesen 0, id: %d, value:%d\n", my_sem0, semctl(my_sem0,0,GETVAL));
		 // printf("Semaphore schreiben 1, id: %d, value:%d\n", my_sem1, semctl(my_sem1,0,GETVAL));

		//Na dann Versuch ich auf den Speicher zuzugreifen:
		int index = 0;
		//printf("Ausgabeversuch: %c\n", *(shmptr + index++));
		do
		{
			//printf("1");
			if (P(my_sem0) != 0)
			{
				/* syscall von einem Signal unterbrochen, wird weiter versucht */
				if (errno == EINTR)	continue;
				error_at_line(0, errno,__FILE__,__LINE__,"%s", strerror(errno));
				break;
			}
			printf("%c", *(shmptr + index));

			index ++;
			V(my_sem1);
			if (index >= puffer) index =0;
		}while (*(shmptr + index) != EOF);
			// Ende erreicht!
			//if (*(shmptr + index) == EOF)	break;
		
	/* Ende des Programms, ressourcen werden aufgeräumt */
		if (shmdt(shmptr) == -1)
		{
			printf("Detach war nicht erfolgreich.");
		}

	} // ende if only_clean
	
	if (testmodus != 1)	printf("Clean sucess: %d\n", clean_all_ext(my_shm, my_sem0, my_sem1));
	else printf("TESTMODUS, no cleaning\n");

	return 0;

	

}	
 

