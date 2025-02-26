#include "libs/crc.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAXSIZE 255
#define CRC_TYPE CRC12_POLY

char time_str[64];
char sbuff[MAXSIZE];
int sockfd;
struct sockaddr_in serveraddr;

char *get_time()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(time_str, sizeof(time_str), "%c", tm);
    return time_str;
}

void *receive_messages()
{
    while (1)
    {
        int recedbytes = recv(sockfd, sbuff, sizeof(sbuff), 0);
        size_t length = strlen(sbuff);                           // Assuming CRC is 4 hex characters
        uint16_t received_crc = strtol((sbuff + length) - 4, NULL, 16); // Get the received CRC value
        for (size_t i = length - 4; i < length; i++)
        {
            sbuff[i] = '\0';
        } 
        uint16_t computed_crc = calculate_crc(sbuff, length - 4, CRC_TYPE);
        printf("[%s - %s:%d | %s - %04x == %04x] Server Message: ", get_time(), (char *)inet_ntoa((struct in_addr)serveraddr.sin_addr), (int)ntohs(serveraddr.sin_port), computed_crc == received_crc ? "SECURE" : "CORRUPT", computed_crc, received_crc);
        puts(sbuff);
    }
}

int main()
{
    int retval;

    char buff[MAXSIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket Creation Error");
        exit(-1);
    }
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("Connection error");
        exit(-1);
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, &receive_messages, NULL);

    while (1)
    {
        scanf("%[^\n]%*c", buff);
        append_crc(buff, strlen(buff), CRC_TYPE);
        int sentbytes = send(sockfd, buff, sizeof(buff), 0);
        if (strcmp(buff, ":exit") == 0 || sentbytes == -1)
        {
            printf("Exited...\n");
            break;
        }
    }

    pthread_cancel(thread_id);
    close(sockfd);
}
