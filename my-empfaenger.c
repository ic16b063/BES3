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
 // #include <sys/wait.h>
 // #include <errno.h>
 // #include <string.h>
 // #include <stdlib.h>

 
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
 
 
int main (int argc, const char *argv[])
{
	if (argc > 2)
	{
		
	}
	
	
	printf("Funktionstest %s\n", argv[0]);
	
	int my_uid = getuid();
	
	int my_shm = create_shm(my_uid);
	
	printf("UID: %d, Shared Memory ID: %d\n", my_uid, my_shm);
	
	int my_sem1 = create_sem(my_uid, 1);
	
	printf("UID: %d, Semaphore 1 ID: %d\n", my_uid, my_sem1);
	
	int my_sem2 = create_sem(my_uid, 0);
	
	printf("UID: %d, Semaphore 2 ID: %d\n", my_uid, my_sem2);
		
	printf("Clean sucess: %d", clean_all(my_shm, my_sem1, my_sem2));
	
	return 1;

	

}	
 

