/*
 * @file sender.c
 * Betriebssysteme shared memory
 * Beispiel 3
 *
 * @author Karl Heinz Steinmetz, Ieriel Stanescu, Christian Posch
 * @date 2017/05/21
 *
 * @version 1.0.0
 *
 * @todo
 * Programm soll die Funktionen
 * popen
 * und 
 * pclose
 * imitieren
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
  #include <stdint.h>
 
  #include <string.h>
  #include <stdlib.h>
  #include <limits.h>
  
  #include <error.h>
  #include <errno.h>
 


 /*
 * --------------------------------------------------------------- defines --
 */
 
 
/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------------------- globals --
 */
	
/*
 * ------------------------------------------------------------- functions --
 */

/**
 * \brief myopen imitiert popen

 * \param command ist ein NULL terminierter Befehlsstring, der die Shellbefehle beinhaltet.
 * \param type ist bestimmt ob schreibend oder lesend ge�ffnet wird. Muss ein NULL terminierter String sein, der nur einen Char beinhalten darf. "w" f�r schreibenden, "r" f�r lesenden Zugriff.
 *
 * \return FILE
 * \retval Bei Erfolg ein Pointer zu einem FILE Struct ansonst NULL.
 */
 int main(int argc, char * const * argv)
 {
    
	uint ringpuffer_index = 0;
	
	// Einlesen der Parameter mittels getopt	
	int opt = 0; // speichert den Charakter der Option (nur ein Zeichen m�glich)
	char *endptr; // zeigt beim strtol auf den Rest des Strings
	uint puffer = 0; // Gr��e (Anzahl an Feldern) des Ringpuffers
	if (argc == 1)
	{
		fprintf(stderr, "%s : ringbuffersize must be spezified\n", argv[0]); //strerror(errno));
		fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]); //strerror(errno));	
		return -1;
	}
	
	// mit Schleife alle Parameter durchgehen bis m gefunden wurde
	while ((opt = getopt(argc, argv, "m:")) != -1)
	{
		if (optind != argc){
			//fprintf(stderr, "%s : ringbuffersize must be spezified\n", argv[0]);
			fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]);
			return -1;
		}
 
		
		switch (opt)
		{
			case 'm':
				errno = 0;
				puffer = (uint) strtol(optarg, &endptr, 10);
				//printf("SIZEMAX: %u, /4 : %u",SIZE_MAX, SIZE_MAX/4);
				//printf("ERRNO: %d, ERANGE: %d, SIZEMAX/4: %u, puffer: %u\n",errno, ERANGE, SIZE_MAX/4, puffer);
				
				//if((errno == ERANGE) || (errno != 0 && puffer == 0) || (errno == 0 && puffer <= 0))
				if(errno == ERANGE || puffer > (SIZE_MAX/4) || (errno != 0 && puffer == 0) || (errno == 0 && puffer <= 0))
				{
					print_usage(argv[0]);
					clean_all_auto();
					return -1; //exit(EXIT_FAILURE);
				}
				/* if((puffer >= 1073741823 || puffer == LONG_MIN))
				// {
					// print_usage(argv[0]);
					// return -1; //exit(EXIT_FAILURE);					
				// }
				//*/
				
				if (*endptr>9)
				{
					fprintf(stderr, "Cannot parse %s\n", optarg);
					print_usage(argv[0]);
					clean_all_auto();
					return -1;
				}
	
				break;
			// case 't':
				// if (only_clean == 0) testmodus = 1;
				// break;
			// case 'c':
				// if (testmodus == 0)	only_clean = 1;
				// break;
			
			default: /* ? */
				//errno = EINVAL;
				if (opt == '?') fprintf(stderr, "%s : Unknown option \"?\" encountered!\n", argv[0]); //strerror(errno));
				fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]); //strerror(errno));
				//error_at_line(0, errno,__FILE__,__LINE__,"%s", strerror(errno));
				return -1;
				//exit(EXIT_FAILURE);
		}
		
	}
	
	
	//printf("Puffergr��e soll %d sein.\n",puffer);
	
	/* checken ob beim Aufruf des Programms die Groesse des Puffers uebergeben wurde */
	/* Checkfunktion ist noch zu erstellen */
	// int ringpuffer_groesse = check_param(argc, argv);
	
	
	/* Variable, die die einzelne Zeichen vom STDIN speichert */
    int eingabe = -1;
    //char *eingabe;
	
    /* Resourcen anlegen */
	int shared_mem_id = create_shm(puffer);
	int sem_schreiben_id = create_sem1(puffer);
	int sem_lesen_id = create_sem0(0);
	
	// printf("Versuche Semaphoren Werte auszugeben:\n");
		
    // printf("Semaphore schreiben 1, id: %d, value:%d\n", sem_schreiben_id, semctl(sem_schreiben_id,0,GETVAL));
	// printf("Semaphore lesen 0, id: %d, value:%d\n", sem_lesen_id, semctl(sem_lesen_id,0,GETVAL));
	// printf("Shared Memory ID: %d\n", shared_mem_id);
	
	int *shm_ptr = shmat(shared_mem_id, NULL, 0);
	/* Shared Memory einblenden */
	if(shm_ptr == (int *) -1)
	{
		printf("Fehler attach shared memory!\n");
		clean_all_ext(shared_mem_id, sem_schreiben_id, sem_lesen_id);
		return -1; //(EXIT_FAILURE);
	}
	
	
	printf("Versuche Semaphoren Werte auszugeben:\n");
		
	printf("Semaphore lesen 0, id: %d, value:%d\n", sem_lesen_id, semctl(sem_lesen_id,0,GETVAL));
	printf("Semaphore schreiben 1, id: %d, value:%d\n", sem_schreiben_id, semctl(sem_schreiben_id,0,GETVAL));

	
    /* Daten von STDIN lesen und in den Ringpuffer speichern */
	do
    {
        eingabe = fgetc(stdin);
		/* Fehlerbehandlung vom Stream */
        if (ferror(stdin))
        {
            perror("Fehler beim Lesen von STDIN");
            clean_all_ext(shared_mem_id, sem_schreiben_id, sem_lesen_id);
            return (EXIT_FAILURE);
        }
		

		printf("Semaphore schreiben 1, id: %d, value:%d\n", sem_schreiben_id, semctl(sem_schreiben_id,0,GETVAL));
        if (P(sem_schreiben_id) != 0) 
		{
			/* syscall von einem Signal unterbrochen, wird weiter versucht */
			if (errno == EINTR)	continue;
			perror("P(sem_schreiben_id)");
			break;
		}

		/* Hier werden Daten in den Puffer reingeschrieben */
		//shm_ptr[ringpuffer_index] = eingabe;
		*(shm_ptr + ringpuffer_index) = eingabe;

		
		if (V(sem_lesen_id) != 0) 
		{
			perror("V(sem_lesen_id)");
			break;
		}
		
		// if (eingabe == EOF)
		// {
			// /*printf("ich hab doch noch kein EOF!\n");
			// break;
		// }

		ringpuffer_index ++;
		if(ringpuffer_index >= puffer) ringpuffer_index = 0;
	}while (eingabe != EOF);
	*(shm_ptr + ringpuffer_index) = EOF;
			// Ende erreicht!
			
	/* Shared Memory ausblenden */
	if (shmdt(shm_ptr) == -1)
	{
		clean_all_auto();
		return EXIT_FAILURE;
	}
	
	// Programmende, Ressourcen aufr�umen muss der Empf�nger �bernehmen!
	
	
    return 0;
 }


 
 
/*
 * =================================================================== eof ==
 */