#include "grp_0-shared.h"

/*key_base soll für alle Berechnungen der Keys verwendet werden.
	Die Struktur wird derart verwendet, dass nach der ersten Berechnung eines Keys, die key_base nicht mehr verändert wird */
static int key_base = 0;
/* von außen nicht verwendbare Berechnung des Keys */
// old: static int my_key(int key_passphrase, int key_nr); 
static int my_key(int key_nr); 


void print_usage(char * fullpath){
	 		fprintf(stderr, "Usage: %s [-h] -m <ring buffer size> \n", fullpath);
 }

void set_key_base(int Key_Base)
{
	//printf("key_base wird gesetzt: %d\n", Key_Base);
	//Setzen des Standardkeys wird nur beim ersten Aufruf durchgeführt
	if (key_base==0)
	{
		if (Key_Base == 0)
		{
			key_base = getuid();
		}
		else
		{
			key_base = Key_Base;
		}
	}
}

static int my_key(int key_nr)
{
	if (key_base == 0) set_key_base(0);
	//printf("keyausgabe: key nr: %d, key_base: %d, errechneter key: %d\n", key_nr,key_base, key_base * 1000 + key_nr);
	return (key_base * 1000 + key_nr);
}


int create_shm(int mem_size)
{
	return create_shm_ext(key_base, mem_size);
}

int create_shm_ext(int key, int mem_size)
{
	//Falls kein Key übergeben wurde (==0), wird auf Standardwert gesetzt, ansonsten wird versucht den Standardwert zu setzen.
	if (key ==0 && key_base != 0) key = key_base;
	else set_key_base(key);
	// Falls 0 als Größe übergeben wird, wird dies auf 1 gesetzt.
	if (mem_size==0) mem_size = 1;
	
	//Key für Shared Memory mit ID 2 um mit Vergleichsprogrammen kompatibel zu sein
	int shm_key = my_key(2);
	errno = 0;

	//printf("short before create sh, key: %d\n",shm_key);
	
	int shm_id = shmget(shm_key, (sizeof(int)*mem_size), 0777 | IPC_CREAT | IPC_EXCL);	
	if (shm_id == -1 )
	{
		if (errno == EEXIST){
			shm_id = shmget(shm_key, sizeof(int)*mem_size, 0777 | IPC_EXCL);
			if (shm_id == -1 )
			{
				if (errno == EACCES)
				{
					printf("Keine Berechtigung auf shared memory\n");
					return -1;
				}
				else printf("Fehler beim Zugriff auf shared memory\n");
				return -1;
			}
			//printf("Shared Memory existiert bereits. ID: %d\n", shm_id);
			return shm_id;
		}
		printf("Fehler beim Erstellen des shared memory.\n");		
		return -1;
	}
	return shm_id;	
}


int create_sem0(int start_value)
{
	return create_sem_ext(key_base, 0, start_value);
}

int create_sem1(int start_value)
{
	return create_sem_ext(key_base, 1, start_value);
}

int create_sem_ext(int key, int key_nr, int start_value)
{
	//Key für Semaphore mit ID 0 und ID 1 um mit Vergleichsprogrammen kompatibel zu sein
	if (key ==0 && key_base != 0) key = key_base;
	else set_key_base(key);
	
	// ergibt bei  key_nr == 0 --> key 1, bei  key_nr == 1 --> key 0
	int sem_key = my_key(key_nr);	
	errno = 0;
	
	int semid = seminit(sem_key, 0666, start_value);
	if (semid == -1)
	{
		semid = semgrab(sem_key);
		if (semid == -1)
		{
			printf("Fehler beim Zugriff auf semaphore");
			return -1;
		}
		return semid;
	}
	return semid;
}


int clean_all_auto()
{
	int sucess=0;
	// löschen shared memory
	sucess = sucess + clean_shm_key(my_key(2));
	// löschen semaphore
	sucess = sucess + clean_sem_key(my_key(0));
	sucess = sucess + clean_sem_key(my_key(1));	
	return sucess;
}

int clean_sem_id(int id)
{
	// löschen semaphoren
	if (semrm(id) == -1 && id != -1)
	{
		//printf("Fehler beim Löschen der Semaphore ID= %d . Bitte manuell mit <IPCS> überprüfen und mit <IPCRM> löschen.\n", id);
		return -1;
	}
	return 0;
}

int clean_sem_key(int key)
{
	//int sem_id = semgrab(key);
	return clean_sem_id(semgrab(key));
}

int clean_shm_key(int key)
{
	// löschen shared memory
	//int shm_id = shmget(key, sizeof(int), 0666 | IPC_EXCL);
	return clean_shm_id(shmget(key, sizeof(int), 0666 | IPC_EXCL));
	
}

int clean_shm_id(int id)
{
	// löschen shared memory
	if (shmctl(id, IPC_RMID, NULL) == -1 && id != -1)
	{
		//printf("Fehler beim Löschen des shared memory ID= %d . Bitte manuell <IPCS> überprüfen und mit <IPCRM> löschen.\n", id);
		return -1;
	}
	return 0;
}


int clean_all_ext(int shm_id, int sem0_id, int sem1_id)
{
	int sucess = 0;
	// löschen shared memory
	if (shm_id==0) sucess += clean_shm_key(my_key(2));
	else sucess += clean_shm_id(shm_id);

	// löschen semaphoren
	if (sem0_id==0) sucess =+ clean_sem_key(my_key(0));
	else sucess =+ clean_sem_id(sem0_id);
	
	if (sem1_id==0)	sucess =+ clean_sem_key(my_key(1));
	else sucess =+ clean_sem_id(sem1_id);
		
	return sucess;
}


