#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

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
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
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

    retval = listen(sockfd, 1);
    if (retval == -1)
    {
        close(sockfd);
        exit(-1);
    }

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

    if (newsockfd == -1)
    {
        close(sockfd);
        exit(-1);
    }

    while (1)
    {
        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1)
        {
            close(sockfd);
            close(newsockfd);
            break;
        }
        printf("[%s] Client Message: ", get_time());
        puts(buff);
        printf("Enter text: ");
        scanf("%[^\n]%*c", buff);
        sentbytes = send(newsockfd, buff, sizeof(buff), 0);
        if (sentbytes == -1)
        {
            close(sockfd);
            close(newsockfd);
            break;
        }
    }

    close(sockfd);
    close(newsockfd);
}
