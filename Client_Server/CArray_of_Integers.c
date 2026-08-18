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
    int i, length;

    key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok failed (make sure 'shmfile' exists)");
        exit(1);
    }

    shmid = shmget(key, 1024, 0666);
    if (shmid == -1) {
        perror("shmget failed (run producer first)");
        exit(1);
    }

    shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat failed to attach shared memory");
        exit(1);
    }

    length = strlen(shared_memory);

    if (length > 0 && shared_memory[length - 1] == '\n')
        length--;

    printf("Consumer read: %s", shared_memory);

    printf("Reversed string: ");
    for (i = length - 1; i >= 0; i--)
    {
        printf("%c", shared_memory[i]);
    }
    printf("\n");

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl RMID failed");
    }

    return 0;
}
