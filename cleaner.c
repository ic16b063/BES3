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
  #include <stdint.h>
  #include <error.h>
  #include <errno.h>

 
int main (int argc, char * const argv[])
{
	uint puffer = 0; // Größe (Anzahl an Feldern) des Ringpuffers	
	int my_shm=0;
	int my_sem0=0;
	int my_sem1=0;
	
	// erstelle shared memmory
	my_shm = create_shm(puffer);
	// erstelle Semaphore 0 (Sender)
	my_sem0 = create_sem0(0);
	// erstelle Semaphore 1 (Empfänger)
	my_sem1 = create_sem1(puffer);
	clean_all_ext(my_shm, my_sem0, my_sem1)/*)*/;
	return 0;
}	
 

