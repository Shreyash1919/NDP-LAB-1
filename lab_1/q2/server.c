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
char result[MAXSIZE];

char *get_time()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(time_str, sizeof(time_str), "%c", tm);
    return time_str;
}

void process_buffer(char *buffer, int len)
{
    int l = 0, r = len - 1;

    int palindrome = 1;
    while (l < r)
    {
        if (buffer[l] != buffer[r])
        {
            palindrome = 0;
            break;
        }
        l++;
        r--;
    }

    int vowels_count[] = {0, 0, 0, 0, 0};
    char vowels[] = {'a', 'e', 'i', 'o', 'u'};
    for (int i = 0; i < 5; i++)
    {
        char ch = tolower(buffer[i]);
        for (int j = 0; j < len; j++)
        {
            if (vowels[i] == tolower(buffer[j]))
            {
                vowels_count[i]++;
            }
        }
    }

    sprintf(result,
            "Is Palindrome: %s - Length: %d - {'a': %d, 'e': %d, 'i': %d, 'o': %d, 'u': %d}",
            palindrome ? "true" : "false",
            len,
            vowels_count[0],
            vowels_count[1],
            vowels_count[2],
            vowels_count[3],
            vowels_count[4]);
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

        if (recedbytes == -1)
        {
            close(sockfd);
            break;
        }
        printf("[%s - %s:%d] Client Message: ", get_time(), (char*)inet_ntoa((struct in_addr)clientaddr.sin_addr), (int) ntohs(clientaddr.sin_port));
        puts(buff);
        process_buffer(buff, strlen(buff));
        printf("result: %s\n", result);
        sentbytes = sendto(sockfd, result, MAXLINE, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)); 
        if (sentbytes == -1)
        {
            close(sockfd);
            break;
        }
    }

    close(sockfd);
}
