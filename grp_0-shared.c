#include "grp_0-shared.h"

/*key_base soll für alle Berechnungen der Keys verwendet werden.
	Die Struktur wird derart verwendet, dass nach der ersten Berechnung eines Keys, die key_base nicht mehr verändert wird */
static int key_base = 0;
/* von außen nicht verwendbare Berechnung des Keys */
static int my_key(int key_passphrase, int key_nr); 

void set_key_base(int Key_Base)
{
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


static int my_key(int key_passphrase, int key_nr)
{
	if (key_passphrase==0) set_key_base(0);
	else if (key_base == 0) set_key_base(key_passphrase);
	return (key_base * 1000 + key_nr);
}

int create_shm(int uid, int mem_size)
{
	if (uid ==0) uid = key_base;
	if (mem_size==0) mem_size = 1;
	
	//Key für Shared Memory mit ID 2 um mit Vergleichsprogrammen kompatibel zu sein
	int shm_key = my_key(uid,2);
	errno = 0;

	int shm_id = shmget(shm_key, (sizeof(int)*mem_size), 0777 | IPC_CREAT | IPC_EXCL);	
	if (shm_id == -1 )
	{
		if (errno == EEXIST){
			shm_id = shmget(shm_key, sizeof(int)*mem_size, 0777 | IPC_EXCL);
			if (shm_id == -1 )
			{
				if (errno == EACCES)
				{
					printf("Keine Berechtigung auf shared memory");
					return -1;
				}
				else printf("Fehler beim Zugriff auf shared memory");
				return -1;
			}
			printf("Shared Memory existiert bereits.");
			return shm_id;
		}
		printf("Fehler beim Erstellen des shared memory");		
		return -1;
	}
	return shm_id;	
}

int clean_all(int shm_id, int sem1_id, int sem2_id)
{
	int sucess = 0;
	// löschen shared memory
	if (shm_id==0)
	{
		int shm_key = my_key(key_base,2);
		shm_id = shmget(shm_key, sizeof(int), 0660 | IPC_EXCL);
	}
	if (sem1_id==0)
	{
		int sem1_key = my_key(key_base,0);
		sem1_id = semgrab(sem1_key);
	}
	if (sem2_id==0)
	{
		int sem2_key = my_key(key_base,1);
		sem2_id = semgrab(sem2_key);
	}
	
	
	if (shmctl(shm_id, IPC_RMID, NULL) == -1 && shm_id != -1)
	{
		printf("Fehler beim Löschen des shared memory ID= %d . Bitte manuell <IPCS> überprüfen und mit <IPCRM> löschen.\n", shm_id);
		sucess--;
	}
	// löschen semaphoren
	if (semrm(sem1_id) == -1 && sem1_id != -1)
	{
		printf("Fehler beim Löschen der Semaphore ID= %d . Bitte manuell <IPCS> überprüfen und mit <IPCRM> löschen.\n", sem1_id);
		sucess--;
	}
	if (semrm(sem2_id) == -1 && sem2_id != -1)
	{
		printf("Fehler beim Löschen der Semaphore ID= %d . Bitte manuell mit <IPCS> überprüfen und mit <IPCRM> löschen.\n", sem2_id);
		sucess--;
	}
	return sucess;
}

int create_sem(int uid, int is_sender)
{
	//Key für Semaphore mit ID 0 und ID 1 um mit Vergleichsprogrammen kompatibel zu sein
	// Zuordnung für Lesen / Schreibend noch unklar
	if (uid ==0) uid = key_base;
	
	int sem_key = my_key(uid,1-is_sender);
	// if (is_sender == 0) sem_key ++;
	// else ++ sem_key ++;
	
	errno = 0;
	
	int semid = seminit(sem_key, 0660, 42);
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


