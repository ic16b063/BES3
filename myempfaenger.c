#include "grp_0-shared.h"


int main (int argc, char * const argv[])
{
	uint index = 0;
	
	uint puffer_size = get_puffer_size(argc, argv);

	int my_shm = create_shm(puffer_size);
	int sema_empf = create_sem0(0);
	int sema_send = create_sem1(puffer_size);
	int ausgaben = -1;
	
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
			if (errno == EINTR)  continue;
			error_at_line(0, errno,__FILE__,__LINE__,"%s", strerror(errno));
			break;
		}
		
	    ausgaben = *(shmptr + index);
		
		if (V(sema_send) == -1) 
		{
			perror("V(sema_send)");
			break;
		}
		
		if (ausgaben != EOF)
		{
			if(fputc(ausgaben, stdout) == EOF)
			{
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            clean_all_ext(my_shm, sema_send, sema_empf);
            return (EXIT_FAILURE);
			}
		}
		
		if (++index == puffer_size) index = 0;
		errno = 0;
	}while (ausgaben != EOF); 
	
	if (shmdt(shmptr) == -1) printf("Detach war nicht erfolgreich.");
	
	clean_all_ext(my_shm, sema_empf, sema_send);
	
	return 0;
}