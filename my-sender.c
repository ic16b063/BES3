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
 * \param type ist bestimmt ob schreibend oder lesend geöffnet wird. Muss ein NULL terminierter String sein, der nur einen Char beinhalten darf. "w" für schreibenden, "r" für lesenden Zugriff.
 *
 * \return FILE
 * \retval Bei Erfolg ein Pointer zu einem FILE Struct ansonst NULL.
 */
 int main(int argc, char * const * argv)
 {
    
	int ringpuffer_index = 0;
	
	// Einlesen der Parameter mittels getopt	
	int opt = 0; // speichert den Charakter der Option (nur ein Zeichen möglich)
	char *endptr; // zeigt beim strtol auf den Rest des Strings
	int puffer = 0; // Größe (Anzahl an Feldern) des Ringpuffers
	
	// mit Schleife alle Parameter durchgehen bis m gefunden wurde
	while ((opt = getopt(argc, argv, "m:")) != -1)
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
	
	
	//printf("Puffergröße soll %d sein.\n",puffer);
	
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
	
	// Programmende, Ressourcen aufräumen muss der Empfänger übernehmen!
	
	
    return 0;
 }

/*
 * =================================================================== eof ==
 */