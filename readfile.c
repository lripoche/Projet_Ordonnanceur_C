#include "header.h"

void readFile() {
    FILE *file = NULL;
    if((file = fopen("dataset", "r+")) == NULL) {
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
        if(msgsnd(id_queue, &p, sizeof(processus) - 4, 0) == -1) {
            perror("Erreur envoi de message");
            destroyQueues();
            exit(1);
        }
    }
    if(fclose(file) != 0) {
        perror("Erreur fermeture fichier");
    }
}
