#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAXSIZE 255

char time_str[64];

char *get_time() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(time_str, sizeof(time_str), "%c", tm);
    return time_str;
}

int main()
{
    int sockfd, retval;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;
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

    while (1)
    {
        printf("Enter text: ");
        scanf("%[^\n]%*c", buff);
        sentbytes = send(sockfd, buff, sizeof(buff), 0);
        if (sentbytes == -1)
        {
            printf("!!");
            close(sockfd);
            break;
        }
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        printf("[%s] Server Response: ", get_time());
        puts(buff);
    }
    close(sockfd);
}
