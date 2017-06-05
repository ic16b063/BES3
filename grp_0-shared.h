
#ifndef MYSHARE_H
#define MYSHARE_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdio.h>

#include <sem182.h>
#include <sys/sem.h>

#include <unistd.h>
#include <sys/types.h>


void set_key_base(int Key_Base);

int create_shm(int mem_size);
int create_shm_ext(int key, int mem_size);

int create_sem0(int start_value);
int create_sem1(int start_value);
int create_sem_ext(int key, int  key_nr, int start_value);

int clean_all_auto(void);
int clean_sem_id(int id);
int clean_sem_key(int key);
int clean_shm_id(int id);
int clean_shm_key(int key);
int clean_all_ext(int shm_id, int sem0_id, int sem1_id);

#endif