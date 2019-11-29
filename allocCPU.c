#include "header.h"

void initIPC(char *argv[]) {                       //Initialisation de la file de message
    if ((key = ftok(argv[0], 0)) == -1) {
	    perror("Erreur de creation de la clé \n");
	    exit(1);
    }  
    if ((id_queue = msgget(key, 0750 | IPC_CREAT | IPC_EXCL)) == -1) {
	    perror("Erreur de creation de la file\n");
	    exit(1);
    } 
    printf("File de message créée %d \n", key);
}

void generateTableAlloc(){                          //Va generer une table d allocation en fonction du nombre d iteration indique dans la table index_iteration
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
