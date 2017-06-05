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
	// Einlesen der Parameter mittels getopt
	
	int opt;
	char *endptr;
	int puffer = 0; // Größe (Anzahl an Feldern) des Ringpuffers
	int testmodus = 0;
	int only_clean = 0;
	

	opt=0;
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
		my_sem0 = create_sem0(puffer);
		// erstelle Semaphore 1 (Empfänger)
		my_sem1 = create_sem1(0);
		
		// int my_uid = getuid();
		// printf("UID: %d, Shared Memory ID: %d\n", my_uid, my_shm);
		// printf("UID: %d, Semaphore 0 ID: %d\n", my_uid, my_sem0);
		// printf("UID: %d, Semaphore 1 ID: %d\n", my_uid, my_sem1);		
	
		
	
	
		/*Beginne ab hier mit dem eigentlichen Programm*/
		int *shmptr = shmat(my_shm,NULL,0);
		if (shmptr == (int*) -1)
		{
			printf("Attach war nicht erfolgreich.");		
		}
		
		// printf("Versuche Semaphoren Werte auszugeben:\n");
		
		// printf("Semaphore 0: %d\n", semctl(my_sem0,0,GETVAL));
		// printf("Semaphore 1: %d\n", semctl(my_sem1,0,GETVAL));
		
		//Na dann Versuch ich auf den Speicher zuzugreifen:
		int index = 0;
			// puffer = max
		//printf("Ausgabeversuch: %c\n", *(shmptr + index++));
		while (P(my_sem0) != -1)
		{
			//printf("Ausgabeversuch: %c\n", *(shmptr + index++));
			printf("%c", *(shmptr + index++));
			V(my_sem1);
			if (index >= puffer) index =0;
		}
		
		
		if (shmdt(shmptr) == -1)
		{
			printf("Detach war nicht erfolgreich.");
		}
	
	

	/* Ende des Programms, ressourcen werden aufgeräumt */

	} // ende if only_clean
	
	if (testmodus != 1)	printf("Clean sucess: %d\n", clean_all_ext(my_shm, my_sem0, my_sem1));
	else printf("TESTMODUS, no cleaning\n");

	return 0;

	

}	
 

