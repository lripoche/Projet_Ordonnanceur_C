#include "header.h"

void destroyQueues() {  
    if(msgctl(id_queue, IPC_RMID, NULL) == -1) {
        perror("Erreur suppression de la file \n");
        exit(1);
    }
    printf("Suppression de la file %d\n", key);
}

void printProcessus(processus p) {
    printf("Processus n°%d, date de soumission : %d, temps d\'execution : %d, priorite : %ld\n", p.pid, p.date_soumission, p.temps_exec, p.type);
}

