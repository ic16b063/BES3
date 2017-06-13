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
		fprintf(stderr, "%s : ringbuffersize must be spezified\n", argv[0]);
		fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", argv[0]);
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
	int sema_empf = create_sem0(0);
	int sema_send = create_sem1(puffer);
	
	int *shmptr = shmat(my_shm, NULL, SHM_RDONLY);
		
	if (shmptr == (int*) -1)
	{
		printf("Attach war nicht erfolgreich!\n");
		clean_all_ext(my_shm, sema_empf, sema_send);
		return -1;
	}
	
	do
	{
		if (P(sema_empf) == -1)
		{
			/* syscall von einem Signal unterbrochen, wird weiter versucht */
			if (errno == EINTR)	continue;
			error_at_line(0, errno,__FILE__,__LINE__,"%s", strerror(errno));
			break;
		}
		
		putchar(*(shmptr + index));
		
		if (V(sema_send) == -1) 
		{
			perror("V(sema_send)");
			break;
		}		
	
		if (++index >= puffer) index = 0;
	} while (*(shmptr + index) != EOF);
	
	if (shmdt(shmptr) == -1) printf("Detach war nicht erfolgreich.");
	
	clean_all_ext(my_shm, sema_empf, sema_send);
	
	return 0;
}