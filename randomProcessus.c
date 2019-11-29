#include "header.h"

void* randomProcessus (void *inutilise) {       //Cree un processus aleatoire a intervalle regulier
    processus p;
    while (1)
    {
        p.type = getRandomInt(largest_element_cpu);
        p.pid = getRandomInt(10000);
        p.temps_exec = getRandomInt(10);
        p.date_soumission = getRandomInt(10);
        if(msgsnd(id_queue, &p, sizeof(processus) - 4, 0) == -1) {
                    perror("Erreur envoi de message du processus aléatoire \n");
        }
        else {
            printf("Processus aléatoire bien ajouté %d\n", p.pid);
        }
        V(0);
        P(1);
        sleep(1);
    } 
}

int getRandomInt(int intervalle) {
    return (rand() % intervalle);
}
