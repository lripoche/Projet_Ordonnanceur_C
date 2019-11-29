#ifndef _MY_SEMAPHORE_H
#define _MY_SEMAPHORE_H

#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/sem.h>

#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE	
#define SEMPERM 0600		 /* Permission */

struct sembuf sem_oper_P ;  /* Operation P */
struct sembuf sem_oper_V ;  /* Operation V */

long semid;

int initsem(key_t semkey);
void P(int semnum);
void V(int semnum);

#endif
