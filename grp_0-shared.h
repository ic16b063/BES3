
#ifndef MYSHARE_H
#define MYSHARE_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdio.h>

#include <sem182.h>

#include <unistd.h>
#include <sys/types.h>

void set_key_base(int Key_Base);
int create_shm(int uid, int mem_size);
int clean_all(int shm_id, int sem1_id, int sem2_id);
int create_sem(int uid, int is_sender);

#endif