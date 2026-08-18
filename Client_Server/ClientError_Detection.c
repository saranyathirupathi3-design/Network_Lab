#include "header.h"

/* Convert character into 8-bit binary */
void char_to_binary(char ch, char binary[])
{
    int value;
    int i;

    value = (int)ch;

    for (i = 7; i >= 0; i--)
    {
        if (value >= 128)
        {
            binary[7 - i] = '1';
            value = value - 128;
        }
        else
        {
            binary[7 - i] = '0';
        }

        value = value * 2;
    }

    binary[8] = '\0';
}


/* Convert string into binary */
void string_to_binary(char text[], char binary[])
{
    int i;
    char temp[9];

    binary[0] = '\0';

    for (i = 0; text[i] != '\0'; i++)
    {
        char_to_binary(text[i], temp);
        strcat(binary, temp);
    }
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


/* Convert decimal into 8-bit binary */
void decimal_to_binary(int value, char binary[])
{
    int i;

    for (i = 7; i >= 0; i--)
    {
        binary[i] = '0' + (value % 2);
        value = value / 2;
    }

    binary[8] = '\0';
}


/* Calculate 8-bit checksum */
int calculate_checksum(char binary[])
{
    int sum;
    int value;
    int i;
    int j;

    char byte[9];

    sum = 0;

    for (i = 0; i < strlen(binary); i = i + 8)
    {
        for (j = 0; j < 8; j++)
        {
            byte[j] = binary[i + j];
        }

        byte[8] = '\0';

        value = binary_to_decimal(byte);

        sum = sum + value;

        /* Wrap-around carry */
        while (sum > 255)
        {
            sum = (sum % 256) + (sum / 256);
        }
    }

    /* 1's complement */
    return 255 - sum;
}


/* Byte stuffing */
void byte_stuff(char data[], char stuffed[])
{
    int i;
    int j;
    int pos;

    char byte[9];

    pos = 0;

    for (i = 0; i < strlen(data); i = i + 8)
    {
        for (j = 0; j < 8; j++)
        {
            byte[j] = data[i + j];
        }

        byte[8] = '\0';


        /* If byte is FLAG */
        if (strcmp(byte, "01111110") == 0)
        {
            /* Add ESCAPE */
            for (j = 0; j < 8; j++)
            {
                stuffed[pos++] = "01111101"[j];
            }

            /* Add FLAG */
            for (j = 0; j < 8; j++)
            {
                stuffed[pos++] = "01111110"[j];
            }
        }


        /* If byte is ESCAPE */
        else if (strcmp(byte, "01111101") == 0)
        {
            /* Add ESCAPE */
            for (j = 0; j < 8; j++)
            {
                stuffed[pos++] = "01111101"[j];
            }

            /* Add ESCAPE again */
            for (j = 0; j < 8; j++)
            {
                stuffed[pos++] = "01111101"[j];
            }
        }


        /* Normal byte */
        else
        {
            for (j = 0; j < 8; j++)
            {
                stuffed[pos++] = byte[j];
            }
        }
    }

    stuffed[pos] = '\0';
}


/* Error introduction */
void introduce_error(char data[])
{
    int choice;
    int count;
    int position;
    int i;

    printf("\n========================================================\n");
    printf("                 ERROR INTRODUCTION\n");
    printf("========================================================\n");

    printf("\nDo you want to introduce error? (1-Yes / 0-No): ");
    scanf("%d", &choice);

    if (choice == 0)
    {
        printf("\nNO ERROR INTRODUCED\n");
        return;
    }

    printf("\nHow many bits do you want to change? : ");
    scanf("%d", &count);

    for (i = 1; i <= count; i++)
    {
        printf("Which bit position do you want to change? : ");
        scanf("%d", &position);

        if (position >= 0 && position < strlen(data))
        {
            if (data[position] == '0')
            {
                data[position] = '1';

                printf("Bit %d changed: 0 -> 1\n",
                       position);
            }
            else
            {
                data[position] = '0';

                printf("Bit %d changed: 1 -> 0\n",
                       position);
            }
        }
        else
        {
            printf("Invalid bit position!\n");
            i--;
        }
    }

    printf("\nERROR INTRODUCED SUCCESSFULLY\n");
}


/* Main */
int main(int argc, char *argv[])
{
    int socket_fd;
    int port;

    int checksum;
    int length;
    int send_length;

    struct sockaddr_in server;

    char text[MAX];

    char binary[MAX * 8];

    char checksum_binary[9];

    char data_with_checksum[MAX * 8];

    char stuffed[MAX * 16];


    /* Check command line arguments */

    if (argc != 3)
    {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);

        printf("Example: %s 127.0.0.1 8080\n",
               argv[0]);

        return 1;
    }


    port = atoi(argv[2]);


    /* Create socket */

    socket_fd = socket(AF_INET,
                       SOCK_STREAM,
                       0);

    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }


    /* Server address */

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;

    server.sin_addr.s_addr =
        inet_addr(argv[1]);

    server.sin_port =
        htons(port);


    /* Connect to server */

    if (connect(socket_fd,
                (struct sockaddr *)&server,
                sizeof(server)) < 0)
    {
        perror("Connection failed");

        close(socket_fd);

        return 1;
    }


    printf("\n========================================================\n");
    printf("                    BYTE STUFFING\n");
    printf("========================================================\n");

    printf("\nFLAG   : 01111110\n");
    printf("ESCAPE : 01111101\n");


    printf("\n-------------------- SENDER SIDE -----------------------\n");


    /* Input string */

    printf("\nEnter string: ");

    /*
       IMPORTANT:
       No getchar() here.
       Otherwise first character may disappear.
    */

    fgets(text, MAX, stdin);


    /* Remove newline */

    if (strlen(text) > 0 &&
        text[strlen(text) - 1] == '\n')
    {
        text[strlen(text) - 1] = '\0';
    }


    printf("\nOriginal Data:\n");

    printf("%s\n", text);


    /* Convert string to binary */

    string_to_binary(text,
                     binary);


    printf("\nOriginal Data in Binary:\n");

    print_binary(binary);


    /* Calculate checksum */

    checksum =
        calculate_checksum(binary);


    decimal_to_binary(checksum,
                      checksum_binary);


    printf("\nChecksum:\n");

    printf("%s\n",
           checksum_binary);


    /* Original data + checksum */

    data_with_checksum[0] = '\0';

    strcat(data_with_checksum,
           binary);

    strcat(data_with_checksum,
           checksum_binary);


    printf("\nOriginal Data + Checksum:\n");

    print_binary(data_with_checksum);


    /* Byte stuffing */

    byte_stuff(data_with_checksum,
               stuffed);


    printf("\n---------------- BYTE STUFFING ----------------\n");


    printf("\nStuffed Data:\n");

    print_binary(stuffed);


    /* Error introduction */

    introduce_error(stuffed);


    printf("\nModified Stuffed Data:\n");

    print_binary(stuffed);


    /* Send length */

    length = strlen(stuffed);

    send_length = htonl(length);


    if (send(socket_fd,
             &send_length,
             sizeof(send_length),
             0) < 0)
    {
        perror("Sending length failed");

        close(socket_fd);

        return 1;
    }


    /* Send stuffed data */

    if (send(socket_fd,
             stuffed,
             length,
             0) < 0)
    {
        perror("Sending data failed");

        close(socket_fd);

        return 1;
    }


    printf("\nConnected to server successfully.\n");

    printf("PPP data sent successfully.\n");

    printf("Client socket closed.\n");


    close(socket_fd);

    return 0;
}
