#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 255
#define MAXLINE 1000

char time_str[64];

char *get_time()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(time_str, sizeof(time_str), "%c", tm);
    return time_str;
}

void swap(char *x, char *y)  
{  
    char temp;  
    temp = *x;  
    *x = *y;  
    *y = temp;  
}  

void permute(char *a, int l, int r)  
{  
    int i;  
    if (l == r)  
        printf("%s\n", a);  
    else
    {  
        for (i = l; i <= r; i++)  
        {  
            swap((a + l), (a + i));  
            permute(a, l + 1, r);
            swap((a + l), (a + i));  
        }  
    }  
}  

int main()
{
    int sockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;

    bzero(&serveraddr, sizeof(serveraddr)); 

    char buff[MAXSIZE];
    int a = 0;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

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

    printf("Server running on %s:%d\n", (char*)inet_ntoa((struct in_addr)serveraddr.sin_addr), (int) ntohs(serveraddr.sin_port));

    while (1)
    {
        recedbytes = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&clientaddr, &actuallen);
        buff[recedbytes] = '\0';

        if (recedbytes == -1)
        {
            close(sockfd);
            break;
        }
        printf("[%s - %s:%d] Client Message: ", get_time(), (char*)inet_ntoa((struct in_addr)clientaddr.sin_addr), (int) ntohs(clientaddr.sin_port));
        puts(buff);
        permute(buff, 0, strlen(buff) - 1);
    }

    close(sockfd);
}
