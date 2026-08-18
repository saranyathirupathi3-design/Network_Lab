#include "header.h"

/* Receive complete data */
int receive_all(int socket_fd, char data[], int size)
{
    int total;
    int received;

    total = 0;

    while (total < size)
    {
        received = recv(socket_fd,
                        data + total,
                        size - total,
                        0);

        if (received <= 0)
            return -1;

        total = total + received;
    }

    return total;
}

/* Print binary with 8-bit separation */
void print_binary(char binary[])
{
    int i;

    for (i = 0; binary[i] != '\0'; i++)
    {
        printf("%c", binary[i]);

        if ((i + 1) % 8 == 0)
            printf(" ");
    }

    printf("\n");
}

/* Convert 8-bit binary to decimal */
int binary_to_decimal(char binary[])
{
    int value;
    int i;

    value = 0;

    for (i = 0; i < 8; i++)
    {
        value = value * 2;

        if (binary[i] == '1')
            value = value + 1;
    }

    return value;
}

/* Byte destuffing */
void byte_destuff(char stuffed[], char destuffed[])
{
    int i;
    int j;
    int pos;
    char byte[9];

    pos = 0;

    for (i = 0; i < strlen(stuffed); i = i + 8)
    {
        for (j = 0; j < 8; j++)
            byte[j] = stuffed[i + j];

        byte[8] = '\0';

        /* ESCAPE found */
        if (strcmp(byte, "01111101") == 0)
        {
            i = i + 8;

            for (j = 0; j < 8; j++)
                destuffed[pos++] = stuffed[i + j];
        }
        else
        {
            for (j = 0; j < 8; j++)
                destuffed[pos++] = byte[j];
        }
    }

    destuffed[pos] = '\0';
}

/* Checksum verification */
int verify_checksum(char data[], char checksum[])
{
    int sum;
    int value;
    int i;
    int j;
    char byte[9];

    sum = 0;

    /* Add data bytes */
    for (i = 0; i < strlen(data); i = i + 8)
    {
        for (j = 0; j < 8; j++)
            byte[j] = data[i + j];

        byte[8] = '\0';

        value = binary_to_decimal(byte);

        sum = sum + value;

        /* Wrap-around carry */
        while (sum > 255)
        {
            sum = (sum % 256) + (sum / 256);
        }
    }

    /* Add checksum */
    value = binary_to_decimal(checksum);

    sum = sum + value;

    /* Wrap-around carry */
    while (sum > 255)
    {
        sum = (sum % 256) + (sum / 256);
    }

    return sum;
}

/* Binary to string */
void binary_to_string(char binary[], char text[])
{
    int i;
    int j;
    int value;
    int pos;

    pos = 0;

    for (i = 0; i + 7 < strlen(binary); i = i + 8)
    {
        value = 0;

        for (j = 0; j < 8; j++)
        {
            value = value * 2;

            if (binary[i + j] == '1')
                value = value + 1;
        }

        text[pos++] = (char)value;
    }

    text[pos] = '\0';
}

int main(void)
{
    int listen_socket;
    int client_socket;
    int length;
    int i;
    int result;

    struct sockaddr_in server;
    struct sockaddr_in client;

    socklen_t client_length;

    char stuffed[MAX * 16];
    char destuffed[MAX * 16];

    char data[MAX * 8];
    char received_checksum[9];

    char received_string[MAX];

    char result_binary[9];

    /* Socket */
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    /* Server address */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(8080);

    /* Bind */
    if (bind(listen_socket,
             (struct sockaddr *)&server,
             sizeof(server)) < 0)
    {
        perror("Binding failed");
        return 1;
    }

    /* Listen */
    if (listen(listen_socket, 5) < 0)
    {
        perror("Listen failed");
        return 1;
    }

    printf("\n========================================================\n");
    printf("                    BYTE STUFFING\n");
    printf("========================================================\n");

    printf("\nFLAG   : 01111110\n");
    printf("ESCAPE : 01111101\n");

    printf("\nTCP Server started...\n");
    printf("Waiting for client...\n");

    /* Accept */
    client_length = sizeof(client);

    client_socket = accept(listen_socket,
                           (struct sockaddr *)&client,
                           &client_length);

    if (client_socket < 0)
    {
        perror("Accept failed");
        return 1;
    }

    printf("\nClient connected successfully.\n");

    /* Receive length */
    receive_all(client_socket,
                (char *)&length,
                sizeof(length));

    length = ntohl(length);

    /* Receive stuffed data */
    receive_all(client_socket,
                stuffed,
                length);

    stuffed[length] = '\0';

    printf("\n---------------- RECEIVER SIDE ----------------\n");

    printf("\nReceived Stuffed Data:\n");
    print_binary(stuffed);

    /* Destuff */
    byte_destuff(stuffed, destuffed);

    printf("\n---------------- BYTE DESTUFFING ----------------\n");

    printf("\nAfter Byte Destuffing:\n");
    print_binary(destuffed);

    /*
       Last 8 bits = checksum
       Remaining bits = original data
    */

    for (i = 0; i < 8; i++)
    {
        received_checksum[i] =
            destuffed[strlen(destuffed) - 8 + i];
    }

    received_checksum[8] = '\0';

    for (i = 0; i < strlen(destuffed) - 8; i++)
    {
        data[i] = destuffed[i];
    }

    data[strlen(destuffed) - 8] = '\0';

    printf("\nOriginal Data in Binary:\n");
    print_binary(data);

    printf("\nReceived Checksum:\n");
    printf("%s\n", received_checksum);

    /* Checksum */
    printf("\n========================================================\n");
    printf("                 CHECKSUM VERIFICATION\n");
    printf("========================================================\n");

    result = verify_checksum(data,
                             received_checksum);

    decimal_to_binary:
    for (i = 7; i >= 0; i--)
    {
        result_binary[i] = '0' + (result % 2);
        result = result / 2;
    }

    result_binary[8] = '\0';

    printf("\nFinal Result:\n");
    printf("%s\n", result_binary);

    if (strcmp(result_binary, "11111111") == 0)
    {
        printf("\nCHECKSUM VALID\n");
        printf("NO ERROR DETECTED\n");
    }
    else
    {
        printf("\nCHECKSUM INVALID\n");
        printf("ERROR DETECTED\n");
    }

    /* Binary to string */
    binary_to_string(data,
                     received_string);

    printf("\n========================================================\n");
    printf("                  RECEIVED DATA\n");
    printf("========================================================\n");

    printf("\nBinary:\n");
    print_binary(data);

    printf("\nString:\n");
    printf("%s\n", received_string);

    close(client_socket);
    close(listen_socket);

    printf("\nServer socket closed.\n");

    return 0;
}
