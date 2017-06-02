
#ifndef MYSHARE_H
#define MYSHARE_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sem182.h>
#include <errno.h>

int get_key(int key_passphrase, int key_nr);
int create_shm(int uid);
void clean_all(int shm_id, int sem1_id, int sem2_id);
int create_sem(int uid, is_sender);

#endif