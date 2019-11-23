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

#define QUEUES_LEN 2

typedef struct {
    long type;
    pid_t pid;
    int temps_exec;
    int date_soumission;
} processus;

int id_queues[QUEUES_LEN]; /* 0: Wait queue, 1 queue courante */ 
key_t keys[10];
key_t key;
//int cpu[] = { 0, 2, 3, 5, 6, 8, 4, 1, 7, 9 };
int cpu[] = { 1, 2, 3 };
processus process[10];
processus processFromFile[7];
processus proc;


void printProcessus(processus p) {
    printf("Processus n°%d, date de soumission : %d, temps d\'execution : %d, priorite : %ld\n", p.pid, p.date_soumission, p.temps_exec, p.type);
}

void initTableCpu(char *argv[]) {
    int i;
    for(i = 0; i < QUEUES_LEN; i++) {
        if ((key = ftok(argv[0], i)) == -1) {
	        perror("Erreur de creation de la clé \n");
	        exit(1);
        }  
     /*   if ((keys[i] = ftok(argv[0], 1)) == -1) {
	        perror("Erreur de creation de la clé \n");
	        exit(1);
        }   */
        //if ((id_queues[i] = msgget(IPC_PRIVATE, 0600)) == -1) {
        if ((id_queues[i]  = msgget(keys[i], 0750 | IPC_CREAT | IPC_EXCL)) == -1) {
	        perror("Erreur de creation de la file\n");
	        exit(1);
        } 
        printf("File de message %d créée \n", id_queues[i]);
    }

}

void destroyQueues() {
    int i;
    for(i = 0; i < QUEUES_LEN; i++) {
       if((id_queues[i] != 0)) {
            if(msgctl(id_queues[i], IPC_RMID, NULL) == -1) {
                perror("Erreur suppression de la file \n");
                exit(1);
            }
            printf("Suppression de la file %d\n", id_queues[i]);
       }
    }
}

void readInQueue(int id, long type) {  //serveur
    processus p;
    if ((msgrcv(id, &p, sizeof(processus) - 4, type, 0)) == -1) {
        perror("Erreur de lecture requete \n");
        destroyQueues();
        exit(1);
    }
     printf("Message recu : %ld", p.type);
}

void putInQueue(int id, processus p) {  //client
    printf("Envoi du message ");
    printProcessus(p);
    if(msgsnd(id, &p, sizeof(processus) - 4, 0) == -1) {
        perror("Erreur envoi de message");
        destroyQueues();
        exit(1);
    }
    //readInQueue(id_queues[1], 7);
}

void forkQueues(processus p) {
    int status;
    processus reponse;
    if(!fork()) {  //fils
        if(msgsnd(id_queues[1], &p, sizeof(processus) - 4, 0) == -1) {
            perror("Erreur envoi de message");
            destroyQueues();
            exit(1);
        }
        if ((msgrcv(id_queues[1], &reponse, sizeof(processus) - 4, 0, 0)) == -1) {
            perror("Erreur de lecture requete \n");
            destroyQueues();
            exit(1);
        }   
        printf("Processus traité : ");
        printProcessus(reponse);
        exit(0);
    } else {
        if ((msgrcv(id_queues[1], &p, sizeof(processus) - 4, 0, 0)) == -1) {
            perror("Erreur de lecture requete \n");
            destroyQueues();
            exit(1);
        }   
        printf("Processus recu par le serveur : ");
        printProcessus(p);
        reponse.pid = 1;
        reponse.temps_exec = p.temps_exec - 1;  //Enleve 1 quantum de temps 
        reponse.type = p.type + 1;         //Priorité -1
        //reponse.date_soumission = p.date_soumission;

        if (msgsnd(id_queues[1], &reponse, sizeof(processus) - 4,0) == -1) {
	        perror("Erreur de lecture requete \n");
            destroyQueues();
	        exit(1);
	    }
        wait(&status);
    }
}

void rrAlgorithm() {
        processus p;
        int i = 0;
        int last_element_cpu = (sizeof(cpu)/sizeof(cpu[0])-1)+1;
        while(1) {
            printf("File traitée %d \n", i);

            if(i > 2) i = 0;
            if ((msgrcv(id_queues[1], &p, sizeof(processus) - 4, cpu[i], IPC_NOWAIT)) == -1) {
                //perror("Erreur de lecture requete \n");
                //destroyQueues();
            }  
            
            if(p.temps_exec<=0){
                sleep(1);
                i++;
                continue;}
            else {
                printf("Message recu : ");
                printProcessus(p);
                p.temps_exec -= 1;
                if (p.type <= (cpu[last_element_cpu]))
                {
                    p.type += 1;
                    printf("This is last : %d \n", last_element_cpu);
                }
                
                //printf("Tache accomplie :");
                //printProcessus(p);
                
                if(msgsnd(id_queues[1], &p, sizeof(processus) - 4, 0) == -1) {
                    perror("Erreur envoi de message");
                    destroyQueues();
                    exit(1);
                }
                printf("Processus après traitement : ");
                printProcessus(p);

            }
            i++;
        }
        
    }

int getRandomInt(int intervalle) {
    return (rand() % intervalle);
}

processus createNewProcessus(int pid) {
    processus p;
    srand(time(NULL));
    sleep(1);
    p.temps_exec = getRandomInt(10);
    sleep(1);
    srand(time(NULL));
    p.date_soumission = getRandomInt(3) + 1;
    p.pid = pid;
    printProcessus(p);
    return p;
}

void createProcess() {
    int i, status;
    for(i = 0; i < 10; i++) {
        if(!fork()) {  //Fils
            createNewProcessus(getpid()); 
            exit(0);
        } else {
            wait(&status);
        }
    }
}

void readFile() {
    FILE *file = NULL;
    if((file = fopen("jeu", "r+")) == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }
    char chaine[7] = "";
    int k = 0;
    processus p[7];
    while (fgets(chaine, 7, file) != NULL) // On lit le fichier tant qu'on ne reçoit pas d'erreur (NULL)
    {
        processus p;
        p.pid = k;
        p.date_soumission = atoi(&chaine[0]);
        p.temps_exec = atoi(&chaine[2]);
        p.type = atoi(&chaine[4]);
        processFromFile[k] = p;
        printProcessus(p);
        k++;
        if(msgsnd(id_queues[1], &p, sizeof(processus) - 4, 0) == -1) {
            perror("Erreur envoi de message");
            destroyQueues();
            exit(1);
        }
    }
    if(fclose(file) != 0) {
        perror("Erreur fermeture fichier");
    }
  // putProcessInCurrentQueue();
}


int main(int argc, char *argv[]) {
    srand(time(NULL));
    initTableCpu(argv);
    readFile();
    rrAlgorithm();
    destroyQueues();
   // createProcess();
   // readFile();
    return 0;
}