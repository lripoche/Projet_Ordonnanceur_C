#ifndef PROJET_H
#define PROJET_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define largest_element_cpu 10
#define cpu_len 100

pthread_t t1, t2;

typedef struct {
    long type;
    pid_t pid;
    int temps_exec;
    int date_soumission;
} processus;

key_t key;
int index_iteration[10];
int cpu[100];
int id_queue;


// Functions
void readFile();
void initIPC(char *argv[]);
void generateTableAlloc();
void* randomProcessus (void *inutilise);
void* rrAlgorithm(void *inutilise);
// Utils
void destroyQueues();
void printProcessus(processus p);

#endif
