#include "grp_0-shared.h"

int my_key(int key_passphrase, int key_nr)
{
	return (key_passphrase * 1000 + key_nr);
}

int create_shm(int uid)
{
	if (uid ==0) uid = getuid();
	
	int shm_key = my_key(uid,0);
	errno = 0;

	int shm_id = shmget(shm_key, sizeof(int), 0660 | IPC_CREAT | IPC_EXCL);	
	if (shm_id == -1 )
	{
		if (errno == EEXIST){
			shm_id = shmget(shm_key, sizeof(int), 0660 | IPC_EXCL);
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
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		printf("Fehler beim Löschen des shared memory ID= %d . Bitte manuell <IPCS> überprüfen und mit <IPCRM> löschen.\n", shm_id);
		sucess--;
	}
	// löschen semaphoren
	if (semrm(sem1_id) == -1)
	{
		printf("Fehler beim Löschen der Semaphore ID= %d . Bitte manuell <IPCS> überprüfen und mit <IPCRM> löschen.\n", sem1_id);
		sucess--;
	}
	if (semrm(sem2_id) == -1)
	{
		printf("Fehler beim Löschen der Semaphore ID= %d . Bitte manuell mit <IPCS> überprüfen und mit <IPCRM> löschen.\n", sem2_id);
		sucess--;
	}
	return sucess;
}

int create_sem(int uid, int is_sender)
{
	if (uid ==0) uid = getuid();
	
	int sem_key = my_key(uid,2-is_sender);
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


