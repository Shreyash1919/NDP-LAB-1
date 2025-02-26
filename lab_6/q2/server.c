#include "libs/crc.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 255
#define CRC_TYPE CRC12_POLY

char time_str[64];
int clients[MAXSIZE];
int c_count = 0;
char sbuff[MAXSIZE];

void *send_messages()
{
    while (1)
    {
        scanf("%[^\n]%*c", sbuff);
        append_crc(sbuff, strlen(sbuff), CRC_TYPE);
        for (int i = 0; i < MAXSIZE; i++)
        {
            if (clients[i] != 0)
            {
                send(clients[i], sbuff, sizeof(sbuff), 0);
            }
        }
        bzero(sbuff, MAXSIZE);
    }
}

char *get_time()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(time_str, sizeof(time_str), "%c", tm);
    return time_str;
}

int main()
{
    bzero(clients, MAXSIZE);
    pid_t childpid;
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

    char buff[MAXSIZE];
    int a = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("\nSocket creation error");
        exit(-1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == 1)
    {
        printf("Binding error");
        close(sockfd);
        exit(-1);
    }

    retval = listen(sockfd, 5);
    if (retval == -1)
    {
        close(sockfd);
        exit(-1);
    }

    actuallen = sizeof(clientaddr);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, &send_messages, NULL);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd < 0)
        {
            break;
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        clients[c_count++ % (MAXSIZE + 1)] = newsockfd;

        if ((childpid = fork()) == 0)
        {
            printf("Child process PID: %d, PPID: %d\n", getpid(), getppid());
            close(sockfd);

            while (1)
            {
                int recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
                if (recedbytes <= 0)
                {
                    break;
                }
                size_t length = strlen(buff);                                  // Assuming CRC is 4 hex characters
                uint16_t received_crc = strtol((buff + length) - 4, NULL, 16); // Get the received CRC value
                for (size_t i = length - 4; i < length; i++)
                {
                    buff[i] = '\0';
                }
                uint16_t computed_crc = calculate_crc(buff, length - 4, CRC_TYPE);
                printf("[%s - %s:%d | %s - %04x == %04x] Client Message: ", get_time(), (char *)inet_ntoa((struct in_addr)clientaddr.sin_addr), (int)ntohs(clientaddr.sin_port), computed_crc == received_crc ? "SECURE" : "CORRUPT", computed_crc, received_crc);
                puts(buff);
            }
        }
    }

    pthread_cancel(thread_id);

    close(sockfd);
    close(newsockfd);
}

// gcc -o server server.c libs/crc.c -I. -pthread && ./server