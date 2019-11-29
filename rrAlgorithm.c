#include "header.h"

void* rrAlgorithm(void *inutilise) {            //Va executer l'algorithme round robin sur une file
        processus p;
        int priorite=0;
        int quantum = 1;                        //Permet de gérer le quantum variable
        int count;                              //Priorite courante dans le cas d une priorite inexistante
        for(int i; 1; i=i+quantum) {
            printf("=== Quantum %d ===\n", i);
            if( priorite > cpu_len) {           //Permet de remettre le compteur à 0 s il depasse la taille de la table d'allocation
                priorite = 0;
            }
            printf("  File traitée %d\n", cpu[priorite]);
            P(0);
            count = cpu[priorite];              //Affecte la priorite courante, dans le cas ou la priorite courante n existe pas, a la priorite courante
            while((msgrcv(id_queue, &p, sizeof(processus) - 4, count, IPC_NOWAIT)) == -1) { //Tant qu'on ne trouve pas la priorite courante, on passe a la priorite suivante
                if(count > largest_element_cpu){ //Si on recoit deja les msg de la plus grande priorite, alors on remet le compteur a 0
                    count = 0;
                    V(1);
                }
                else
                {
                    printf("  File traitée %d\n", count);
                    count++;                    //Si la priorite n existe pas, on incremente la priorite courante
                    V(1);
                }         
            }

            V(1);  
            
            if(p.temps_exec<=0) {               //Permet de sortir les processus completement executes
                printf("  Le processus %d a fini son execution \n", p.pid);
                sleep(1);
                continue;
            }
            else {
                printf("  Message recu : ");
                printProcessus(p);
                p.temps_exec -= quantum;
                if (p.type <= largest_element_cpu) //Permet de ne pas mettre une priorite superieure a la priorite max de la table
                {
                    p.type += 1;
                }
                if(msgsnd(id_queue, &p, sizeof(processus) - 4, 0) == -1) {
                    perror("  Erreur envoi de message post traitement");
                    destroyQueues();
                    exit(1);
                }
                printf("  Processus après traitement : ");
                printProcessus(p);
            }
            priorite++;
        }   
    }