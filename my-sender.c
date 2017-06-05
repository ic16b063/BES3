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
	
	/* checken ob beim Aufruf des Programms die Groesse des Puffers uebergeben wurde */
	/* Checkfunktion ist noch zu erstellen */
	int ringpuffer_groesse = check_param(argc, argv);
	
	
	/* Variable, die die einzelne Zeichen vom STDIN speichert */
    int eingabe = -1;
    
    /* Resourcen anlegen */
	int shared_mem_id = create_shm(uid);
	int sem_schreiben_id = create_sem(uid, 1);
	int sem_lesen_id = create_sem(uid, 2);
	
	/* Shared Memory einblenden */
	if((shm_ptr = shmat(shared_mem_id, NULL, SHM_RDONLY)) == (int *) -1);
	{
		clean_all(shared_mem_id, sem_schreiben_id, sem_lesen_id);
		return (EXIT_FAILURE);
	}
    /* Daten von STDIN lesen und in den Ringpuffer speichern */
	for (;;)
    {
        if (eingabe != EOF)
			break;
		eingabe = fgetc(stdin);
		/* Fehlerbehandlung vom Stream */
        if (ferror(stdin))
        {
            perror("Fehler beim Lesen von STDIN");
            clean_all(shared_mem_id, sem_schreiben_id, sem_lesen_id);
            return (EXIT_FAILURE);
        }
			
        if (P(sem_schreiben_id) != 0) 
		{
			if (errno == EINTR) 
			{
				/* syscall von einem Signal unterbrochen, wird weiter versucht */
				continue;
			}
			perror("P(sem_schreiben_id)");
				break;
		}
		/* Hier werden Daten in den Puffer reingeschrieben */
		*shm_ptr[ringpuffer_index] = eingabe;
		
		if (V(sem_lesen_id) != 0) 
		{
			perror("V(sem_lesen_id)");
			break;
		}
		
		ringpuffer_index ++;
		if(ringpuffer_index >= ringpuffer_groesse)
		ringpuffer_index = 0;
	}

	/* Shared Memory ausblenden */
    if (shmdt(shm_ptr) == ­1)
	{
	   return EXIT_FAILURE;
	}

    return 0;
 }

/*
 * =================================================================== eof ==
 */