#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main()
{
    key_t key;
    int shmid;
    char *shared_memory;

    key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok failed (make sure 'shmfile' exists)");
        exit(1);
    }

    shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed to create shared memory");
        exit(1);
    }

    shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat failed to attach shared memory");
        exit(1);
    }

    printf("Enter a string: ");
    fgets(shared_memory, 1024, stdin);

    printf("Producer wrote: %s", shared_memory);

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed to detach memory");
    }

    return 0;
}
