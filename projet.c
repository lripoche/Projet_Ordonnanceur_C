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

#define QUEUES_LEN 2
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

int id_queues[QUEUES_LEN]; /* 0: Wait queue, 1 queue courante */ 
key_t keys[10];
key_t key;
//int cpu[] = { 0, 2, 3, 5, 6, 8, 4, 1, 7, 9 };
int prct[10] = {26,20,16,12,9,7,5,3,2,1};
int cpu[100];
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
        if ((id_queues[i]  = msgget(keys[i], 0750 | IPC_CREAT | IPC_EXCL)) == -1) {
	        perror("Erreur de creation de la file\n");
	        exit(1);
        } 
        printf("File de message %d créée \n", id_queues[i]);
    }
}

void generateTableAlloc(){
    for (int i = 0; i < 10; i++)
    {
        int iteration=prct[i];
        for (int j = 0; j < iteration; j++)
        {
            int n = (sizeof(cpu)/sizeof(cpu[0]))+1;
            cpu[n]=i;
        }
        
    }
    
}

int find_maximum(int arr[], int n) {
      int i; 
     
    // Initialize maximum element 
    int max = arr[0]; 
  
    // Traverse array elements from second and 
    // compare every element with current max   
    for (i = 1; i < n; i++) 
        if (arr[i] > max) 
            max = i; 
  
    return max; 
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

void* rrAlgorithm(void *inutilise) {    //Va executer l'algorithme round robin sur une file
        processus p;
        //int cpu_len = sizeof(cpu)/sizeof(cpu[0]);
        //location = find_maximum(cpu, cpu_len);
        //largest_element_cpu  = cpu[location];
        int priorite=0;
        int quantum = 1; //Permet de gérer le quantum variable
        int count;
        for(int i; 1; i=i+quantum) {
            printf("=== Quantum %d ===\n", i);
            if( priorite > cpu_len) { //Permet de remettre le compteur à 0 si il depasse la taille de la table d'allocation
                printf("  balbal 1 \n");
                priorite = 0;
            }
            printf("  File traitée %d %d\n", priorite, largest_element_cpu);
            P(0);
            count = cpu[priorite];  // priorité courante
            while((msgrcv(id_queues[1], &p, sizeof(processus) - 4, count, IPC_NOWAIT)) == -1) { //Permet, dans le cas ou le la file est vide, passer à la file suivante
            //perror("Erreur de lecture requete \n");
            if(count > largest_element_cpu){ //Si on recoit deja les msg de la plus grande priorite, alors on remet le compteur a 0
                printf("  balbal 2 \n");

                count = 0;
                V(1);
            }
            else
            {
                printf("  File traitée jhhjhj %d\n", count);

                count++;
                V(1);
            }         
            }
                

            
            
            V(1);  
            
            if(p.temps_exec<=0) { //Permet de sortir les processus completement executes
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
                    printf("  This is last : %d \n", largest_element_cpu);
                }
                
                //printf("Tache accomplie :");
                //printProcessus(p);
                
                if(msgsnd(id_queues[1], &p, sizeof(processus) - 4, 0) == -1) {
                    perror("  Erreur envoi de message");
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
}

void* randomProcessus (void *inutilise) {       //Cree un processus aleatoire toutes les secondes
    //location = find_maximum(cpu, sizeof(cpu));
    //largest_element_cpu  = cpu[location];
    processus p;
    while (1)
    {
        p.type = getRandomInt(largest_element_cpu);
        p.pid = getRandomInt(10000);
        p.temps_exec = getRandomInt(10);
        p.date_soumission = getRandomInt(10);
        printf("Random que l'on va ajouter %d\n",p.pid);
        
        if(msgsnd(id_queues[1], &p, sizeof(processus) - 4, 0) == -1) {
                    perror("Erreur envoi de message du random \n");
                    //destroyQueues();
                    //exit(1);
        }
        else {
            printf("Random bien ajouté %d\n", p.pid);
        }
        V(0);
        P(1);
        sleep(1);
    }
    
}


int main(int argc, char *argv[]) {
    srand(time(NULL));
    initTableCpu(argv);
    generateTableAlloc();
    // Implementation des threads
    pthread_create(&t1, NULL, readFile, NULL);
    pthread_create(&t2, NULL, rrAlgorithm, NULL);
    pthread_create(&t3, NULL, randomProcessus, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);


    //readFile();
    //rrAlgorithm();
    destroyQueues();
   // createProcess();
   // readFile();
    return 0;
}