#include "grp_0-shared.h"


int main (int argc, char * const argv[])
{
	uint index = 0;
	
	uint puffer_size = get_puffer_size(argc, argv);
	
	int my_shm = create_shm(puffer_size);
	int sema_send = create_sem1(puffer_size);
	int sema_empf = create_sem0(0);
	int eingabe = -1; //Variable, die die einzelne Zeichen vom STDIN speichert
	
	int *shmptr = shmat(my_shm, NULL, 0); //Shared Memory einblenden
	
	if(shmptr == (int *) -1)
	{
		printf("Attach war nicht erfolgreich!\n");
		clean_all_ext(my_shm, sema_send, sema_empf);
		return -1;
	}
	
    /* Daten von STDIN lesen und in den Ringpuffer speichern */
	do
    {
        eingabe = fgetc(stdin);
		if (ferror(stdin))
        {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            clean_all_ext(my_shm, sema_send, sema_empf);
            return (EXIT_FAILURE);
        }
        
		if (P(sema_send) == -1) 
		{
			/* syscall von einem Signal unterbrochen, wird weiter versucht */
			if (errno == EINTR)	 continue;
			perror("P(sema_send)");
			break;
		}

		/* Hier werden Daten in den Puffer reingeschrieben */
		*(shmptr + index) = eingabe;
		
		if (V(sema_empf) == -1) 
		{
			error_at_line(0, errno,__FILE__,__LINE__,"%s", strerror(errno));
			break;
		}
		
		if (++index == puffer_size) index = 0;	
        errno = 0;
	} while (eingabe != EOF);
			
	/* Shared Memory ausblenden */
	if (shmdt(shmptr) == -1)
	{
		clean_all_auto();
		return EXIT_FAILURE;
	}
    return 0;
 }