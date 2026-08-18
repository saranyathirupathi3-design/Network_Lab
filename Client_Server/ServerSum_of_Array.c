#include "header.h"

int main(void)
{
    int ls;
    int s;

    int arr[MAX];
    int n;
    int i;
    int sum;

    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;

    socklen_t clientAddrLen;

    /* Server address */

    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_PORT);

    /* Create socket */

    ls = socket(AF_INET, SOCK_STREAM, 0);

    if (ls < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    /* Bind */

    if (bind(ls,
             (struct sockaddr *)&servAddr,
             sizeof(servAddr)) < 0)
    {
        perror("Binding failed");
        exit(1);
    }

    /* Listen */

    if (listen(ls, 16) < 0)
    {
        perror("Listening failed");
        exit(1);
    }

    printf("TCP Sum Server started...\n");
    printf("Waiting for client...\n");

    /* Accept */

    clientAddrLen = sizeof(clientAddr);

    s = accept(ls,
               (struct sockaddr *)&clientAddr,
               &clientAddrLen);

    if (s < 0)
    {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected.\n");

    /* Receive number of elements */

    recv(s, &n, sizeof(n), 0);

    /* Receive array */

    recv(s, arr, sizeof(int) * n, 0);

    printf("\nArray received from client:\n");

    for (i = 0; i < n; i++)
    {
        printf("%d", arr[i]);

        if (i < n - 1)
            printf(",");
    }

    printf("\n");

    /* Calculate sum */

    sum = 0;

    for (i = 0; i < n; i++)
    {
        sum = sum + arr[i];
    }

    printf("Sum calculated by server: %d\n", sum);

    /* Send sum to client */

    send(s, &sum, sizeof(sum), 0);

    printf("Sum sent to client.\n");

    /* Close */

    close(s);
    close(ls);

    printf("Server closed.\n");

    return 0;
}
