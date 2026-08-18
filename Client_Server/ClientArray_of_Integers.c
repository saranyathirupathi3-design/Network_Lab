#include "header.h"

int main(void)
{
    int s;

    int arr[MAX];
    int n;
    int i;

    struct sockaddr_in servAddr;

    /* Create socket */

    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    /* Server address */

    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(SERV_PORT);

    /* Connect */

    if (connect(s,
                (struct sockaddr *)&servAddr,
                sizeof(servAddr)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to TCP Server.\n");

    /* Get number of elements */

    printf("Enter number of elements: ");
    scanf("%d", &n);

    /* Get array */

    printf("Enter %d elements separated by comma: ", n);

    for (i = 0; i < n; i++)
    {
        scanf("%d", &arr[i]);

        if (i < n - 1)
            scanf(",");
    }

    /* Send number of elements */

    send(s, &n, sizeof(n), 0);

    /* Send array */

    send(s, arr, sizeof(int) * n, 0);

    printf("\nArray sent to server.\n");

    /* Receive number of elements */

    recv(s, &n, sizeof(n), 0);

    /* Receive array */

    recv(s, arr, sizeof(int) * n, 0);

    printf("\nArray received from server:\n");

    for (i = 0; i < n; i++)
    {
        printf("%d", arr[i]);

        if (i < n - 1)
            printf(",");
    }

    printf("\n");

    /* Close */

    close(s);

    printf("Client closed.\n");

    return 0;
}
