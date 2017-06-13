#include "grp_0-shared.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <error.h>
#include <errno.h>

int main (int argc, char * const argv[])
{
	// Einlesen der Parameter mittels getopt	
	int opt = 0; // speichert den Charakter der Option (nur ein Zeichen möglich)
	char *endptr; // zeigt beim strtol auf den Rest des Strings
	uint puffer = 0; // Größe (Anzahl an Feldern) des Ringpuffers
	uint index = 0;
	
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
			fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]);
			return -1;
		}
		
		switch (opt)
		{
			case 'm':
				errno = 0;
				puffer = (uint) strtol(optarg, &endptr, 10);
				if(errno == ERANGE || puffer > (SIZE_MAX/4) || (errno != 0 && puffer == 0) || (errno == 0 && puffer <= 0))
				{
					print_usage(argv[0]);
					clean_all_auto();
					return -1;
				}
				if (*endptr>9)
				{
					fprintf(stderr, "Cannot parse %s\n", optarg);
					print_usage(argv[0]);
					clean_all_auto();
					return -1;
				}
				break;
			default: /* ? */
				if (opt == '?') fprintf(stderr, "%s : Unknown option \"?\" encountered!\n", argv[0]);
				fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]);
				clean_all_auto();
				return -1;
		}	
	}
	
	int my_shm = create_shm(puffer);
	int sema_send = create_sem1(puffer);
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
			if (errno == EINTR)	continue;
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
		
		if (eingabe == EOF)
		{
			break;
		}
		
		if (++index >= puffer) index = 0;	
	} while (eingabe != EOF);
	
	*(shmptr + index) = EOF;
			// Ende erreicht!
			
	/* Shared Memory ausblenden */
	if (shmdt(shmptr) == -1)
	{
		clean_all_auto();
		return EXIT_FAILURE;
	}
    return 0;
 }