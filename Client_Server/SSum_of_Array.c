#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct message
{
    long msg_type;
    char msg_text[100];
};

int main()
{
    key_t key;
    int msgid;
    struct message msg;
    int i, count = 0;
    int in_word = 0;

    key = ftok("msgfile", 65);
    if (key == -1) {
        perror("ftok failed (make sure 'msgfile' exists)");
        exit(1);
    }

    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget failed (run p2 first)");
        exit(1);
    }

    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0) == -1) {
        perror("msgrcv failed to receive message");
        exit(1);
    }

    printf("Consumer received: %s", msg.msg_text);

    for (i = 0; msg.msg_text[i] != '\0'; i++)
    {
        if (msg.msg_text[i] != ' ' &&
            msg.msg_text[i] != '\n' &&
            msg.msg_text[i] != '\t')
        {
            if (in_word == 0)
            {
                count++;
                in_word = 1;
            }
        }
        else
        {
            in_word = 0;
        }
    }

    printf("Number of words: %d\n", count);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl RMID failed");
    }

    return 0;
}
