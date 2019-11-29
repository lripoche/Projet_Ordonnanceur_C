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
#define cpu_len 10

int inutilise;
pthread_t t1, t2, t3;

//int location,maximum,largest_element_cpu;

typedef struct {
    long type;
    pid_t pid;
    int temps_exec;
    int date_soumission;
} processus;

key_t key;
//int cpu[] = { 0, 2, 3, 5, 6, 8, 4, 1, 7, 9 };
int index_iteration[10] = {26,20,16,12,9,7,5,3,2,1};
int cpu[100];


void printProcessus(processus p) {
    printf("Processus n°%d, date de soumission : %d, temps d\'execution : %d, priorite : %ld\n", p.pid, p.date_soumission, p.temps_exec, p.type);
}

void initIPC(char *argv[]) {
    if ((key = ftok(argv[0], 1)) == -1) {
	    perror("Erreur de creation de la clé \n");
	    exit(1);
    }  
    if ((msgget(1, 0750 | IPC_CREAT | IPC_EXCL)) == -1) {
	    perror("Erreur de creation de la file\n");
	    exit(1);
    } 
    printf("File de message créée %d \n", key);

}

void generateTableAlloc(){                  //Va generer une table d allocation en fonction du nombre d iteration indique dans la table index_iteration
    for (int i = 0; i < 10; i++)
    {
        int iteration=index_iteration[i];
        for (int j = 0; j < iteration; j++)
        {
            int n = (sizeof(cpu)/sizeof(cpu[0]))+1; //Taille actuelle de la table cpu
            cpu[n]=i;
        }
        
    }
    
}

void destroyQueues() {  
    if(msgctl(1, IPC_RMID, NULL) == -1) {
        perror("Erreur suppression de la file \n");
        exit(1);
    }
    printf("Suppression de la file %d\n", key);
}

void* rrAlgorithm(void *inutilise) {            //Va executer l'algorithme round robin sur une file
        processus p;
        int priorite=0;
        int quantum = 1;                        //Permet de gérer le quantum variable
        int count;                              //Priorite courante dans le cas d une file vide
        for(int i; 1; i=i+quantum) {
            printf("=== Quantum %d ===\n", i);
            if( priorite > cpu_len) {           //Permet de remettre le compteur à 0 s il depasse la taille de la table d'allocation
                priorite = 0;
            }
            printf("  File traitée %d\n", priorite);
            P(0);
            count = cpu[priorite];              //Affecte la priorite courante, dans le cas ou la priorite courante n existe pas, a la priorite courante
            while((msgrcv(1, &p, sizeof(processus) - 4, count, IPC_NOWAIT)) == -1) { //Tant qu'on ne trouve pas la priorite courante, on passe a la priorite suivante
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
                printf("  Le processus %d a finit son execution \n", p.pid);
                sleep(1);
                continue;
            }
            else {
                printf("  Message recu : ");
                printProcessus(p);
                p.temps_exec -= 1;
                if (p.type <= largest_element_cpu) //Permet de ne pas mettre une priorite superieure a la priorite max de la table
                {
                    p.type += 1;
                }
                if(msgsnd(1, &p, sizeof(processus) - 4, 0) == -1) {
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

int getRandomInt(int intervalle) {
    return (rand() % intervalle);
}

void* readFile(void *inutilise) {
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
        printProcessus(p);
        k++;
        if(msgsnd(1, &p, sizeof(processus) - 4, 0) == -1) {
            perror("Erreur envoi de message");
            destroyQueues();
            exit(1);
        }
    }
    if(fclose(file) != 0) {
        perror("Erreur fermeture fichier");
    }
}

void* randomProcessus (void *inutilise) {       //Cree un processus aleatoire a intervalle regulier
    processus p;
    while (1)
    {
        p.type = getRandomInt(largest_element_cpu);
        p.pid = getRandomInt(10000);
        p.temps_exec = getRandomInt(10);
        p.date_soumission = getRandomInt(10);
        if(msgsnd(key, &p, sizeof(processus) - 4, 0) == -1) {
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


int main(int argc, char *argv[]) {

    //Initialisation de srand
    srand(time(NULL));

    //Initisalisation de la file
    initIPC(argv);

    //Creation de la table d allocation
    generateTableAlloc();

    // Implementation des threads
    pthread_create(&t1, NULL, readFile, NULL);
    pthread_create(&t2, NULL, rrAlgorithm, NULL);
    pthread_create(&t3, NULL, randomProcessus, NULL);

    //Synchronisation de la fin des threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    //Suppression de la file
    destroyQueues();
    return 0;
}