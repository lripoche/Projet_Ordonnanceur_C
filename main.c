#include "header.h"

//for i in $(ipcs | awk '{print $2}' | sed -n '/msqid/,/Shared/p' | grep -v Shared | grep -v msqid | head -n -1); do ipcrm -q $i; done

int main(int argc, char *argv[]) {
    //Initialisation de srand
    srand(time(NULL));

    //Initisalisation de la file
    initIPC(argv);

    //Creation de la table d allocation
    index_iteration[0] = 26;
    index_iteration[1] = 20;
    index_iteration[2] = 16;
    index_iteration[3] = 12;
    index_iteration[4] = 9;
    index_iteration[5] = 7;
    index_iteration[6] = 5;
    index_iteration[7] = 3;
    index_iteration[8] = 2;
    index_iteration[9] = 1;

    generateTableAlloc();

    //Read File
    readFile();

    // Implementation des threads
    pthread_create(&t1, NULL, rrAlgorithm, NULL);
    pthread_create(&t2, NULL, randomProcessus, NULL);

    //Synchronisation de la fin des threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    //Suppression de la file
    destroyQueues();
    return 0;
}
