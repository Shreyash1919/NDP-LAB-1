#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#define MAXSIZE 90

void bubbleSort_a(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void bubbleSort_d(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] < arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int send_msg(const char *buff, int newsockfd)
{
    int sentbytes;
    sentbytes = send(newsockfd, buff, strlen(buff), 0);
    if (sentbytes == -1)
    {
        printf("Error in sending message\n");
        return -1;
    }
    return 0;
}

void handle_client(int newsockfd, int sockfd)
{
    int recedbytes, sentbytes;
    char buff[MAXSIZE];

    int arr[5];
        int count = 0, option, sub;
        
        // Receive count (number of elements in the array)
        recedbytes = recv(newsockfd, &count, sizeof(count), 0);
        if (recedbytes <= 0)
        {
            return;
        }
        printf("\nCount received: %d", count);

        // Receive the array of integers
        recedbytes = recv(newsockfd, arr, count * sizeof(int), 0);
        if (recedbytes <= 0)
        {
            return;
        }

        printf("\nArray Received: ");
        for (int i = 0; i < count; i++)
        {
            printf("%d ", arr[i]);
        }


    while (1)
    {
        
        // Receive option (what operation to perform)
        recedbytes = recv(newsockfd, &option, sizeof(option), 0);
        if (recedbytes <= 0)
        {
            break;
        }
        printf("\nOption received: %d", option);

        // Receive sub (extra data required for the operation)
        recedbytes = recv(newsockfd, &sub, sizeof(sub), 0);
        if (recedbytes <= 0)
        {
            break;
        }

        printf("\nSub received: %d", sub);

        switch (option)
        {
        case 1: // Search for a number
            for (int i = 0; i < count; i++)
            {
                if (arr[i] == sub)
                {
                    send_msg("Found!", newsockfd);
                    break;
                }
                if (i == count - 1)
                {
                    send_msg("Not found!", newsockfd);
                }
            }
            break;

        case 2:           // Sorting the array
            if (sub == 1) // Ascending order
            {
                bubbleSort_a(arr, count);
            }
            else // Descending order
            {
                bubbleSort_d(arr, count);
            }
            sentbytes = send(newsockfd, arr, count * sizeof(int), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending sorted array\n");
                break;
            }
            break;

        case 3: // Split operation (Odd/Even)
            {
                int even_arr[MAXSIZE], odd_arr[MAXSIZE];
                int even_count = 0, odd_count = 0;

                // Split the array into even and odd numbers
                for (int i = 0; i < count; i++)
                {
                    if (arr[i] % 2 == 0)
                    {
                        even_arr[even_count++] = arr[i];
                    }
                    else
                    {
                        odd_arr[odd_count++] = arr[i];
                    }
                }

                // Send result back to client
                send_msg("Array split into Even and Odd", newsockfd);

                sentbytes = send(newsockfd, even_arr, even_count * sizeof(int), 0);
                if (sentbytes == -1)
                {
                    printf("Error in sending even array\n");
                    break;
                }

                sentbytes = send(newsockfd, odd_arr, odd_count * sizeof(int), 0);
                if (sentbytes == -1)
                {
                    printf("Error in sending odd array\n");
                    break;
                }
            }
            break;
        case 4:
            // close(newsockfd);
            // close(sockfd);
            break;
        default:
            send_msg("Invalid Option", newsockfd);
            break;
        }
        printf("\nTask Completed....");
    }

    close(newsockfd);
}

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        perror("Binding error");
        close(sockfd);
        return -1;
    }

    retval = listen(sockfd, 1);
    printf("Server is listening on port 3388...\n");
    if (retval == -1)
    {
        perror("Listen error");
        close(sockfd);
        return -1;
    }

    actuallen = sizeof(clientaddr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd == -1)
        {
            printf("Accept error\n");
            close(sockfd);
            return -1;
        }
        printf("\nNew client!!\n");

        // Handle client connection
        handle_client(newsockfd, sockfd);
    }

    close(sockfd);
    return 0;
}
