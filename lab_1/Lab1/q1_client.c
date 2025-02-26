#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 50

void handle_comm(int sockfd)
{
    int sentbytes;
    int arr[5];
    int count = 0;
    int option, sub;

    int recedbytes;
    char buff[MAXSIZE];
    printf("Enter set of integers (space-separated, end with Enter): ");
    count = 0;
    while (scanf("%d", &(arr[count])) == 1)
    {
        count++;
        if (count == 5)
            break; // Maximum of 5 integers
    }

    // Clear any leftover input characters from the buffer
    while (getchar() != '\n')
        ;

    // Send the count of integers
    sentbytes = send(sockfd, &count, sizeof(count), 0);
    if (sentbytes == -1)
    {
        printf("Error in sending message (sending count)\n");
        return;
    }

    // Send the array of integers
    sentbytes = send(sockfd, arr, count * sizeof(int), 0);
    if (sentbytes == -1)
    {
        printf("Error in sending message (sending array)\n");
        return;
    }
    while (1)
    {
        printf("Enter operation: 1-Search for number 2-Sort 3-Split(Odd/Even) 4-exit: ");
        scanf("%d", &option);

        sentbytes = send(sockfd, &option, sizeof(option), 0);
        if (sentbytes == -1)
        {
            printf("Error in sending message (sending option)\n");
            break;
        }

        switch (option)
        {
        case 1:
            // Search operation
            printf("Enter number to search: ");
            scanf("%d", &sub);
            sentbytes = send(sockfd, &sub, sizeof(sub), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending message (sending search number)\n");
                break;
            }

            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving message\n");
                break;
            }
            if (recedbytes == 0)
            {
                break; // server closed the connection
            }
            printf("Text Received from Server: ");
            puts(buff);
            break;

        case 2:
            // Sorting operation
            printf("Ascending-1 Descending-2: ");
            scanf("%d", &sub);
            sentbytes = send(sockfd, &sub, sizeof(sub), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending message (sending sort choice)\n");
                break;
            }
            recedbytes = recv(sockfd, arr, count * sizeof(int), 0);
            if (recedbytes <= 0)
            {
                break;
            }

            printf("\nArray Received from server: ");
            for (int i = 0; i < count; i++)
            {
                printf("%d ", arr[i]);
            }
            printf("\n");
            break;

        case 3:
            // Split operation (Odd/Even)
            printf("Odd-1 Even-2: ");
            scanf("%d", &sub);
            sentbytes = send(sockfd, &sub, sizeof(sub), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending message (sending sort choice)\n");
                break;
            }
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving message\n");
                break;
            }
            if (recedbytes == 0)
            {
                break; // server closed the connection
            }
            printf("Text Received from Server: ");
            puts(buff);

            // Receive even array from server
            int even_arr[MAXSIZE], odd_arr[MAXSIZE];
            recedbytes = recv(sockfd, even_arr, sizeof(even_arr), 0);
            if (recedbytes <= 0)
            {
                break;
            }

            printf("Even Numbers Received: ");
            for (int i = 0; i < recedbytes / sizeof(int); i++)
            {
                printf("%d ", even_arr[i]);
            }
            printf("\n");

            // Receive odd array from server
            recedbytes = recv(sockfd, odd_arr, sizeof(odd_arr), 0);
            if (recedbytes <= 0)
            {
                break;
            }

            printf("Odd Numbers Received: ");
            for (int i = 0; i < recedbytes / sizeof(int); i++)
            {
                printf("%d ", odd_arr[i]);
            }
            printf("\n");

            break;

        case 4:
            printf("Exiting...\n");
            close(sockfd);
            exit(0); // Exit the program

        default:
            break;
        }
    }
}

int main()
{
    int sockfd, retval;
    struct sockaddr_in serveraddr;

    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket Creation Error\n");
        return -1;
    }

    // Setup the server address struct
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("Connection error\n");
        close(sockfd);
        return -1;
    }

    // Start communication loop
    while (1)
    {
        handle_comm(sockfd); // Send integer array and handle server communication
    }

    close(sockfd);
    return 0;
}
