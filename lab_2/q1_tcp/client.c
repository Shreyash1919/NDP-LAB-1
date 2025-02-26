#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXSIZE 2000

int sockfd;
struct sockaddr_in serveraddr;
char buff[MAXSIZE];

void close_connections()
{
    close(sockfd);
}

int recieve_msg(char *buff)
{
    int recv_bytes = recv(sockfd, buff, MAXSIZE - 1, 0);
    if (recv_bytes <= 0)
    {
        close_connections();
        return -1;
    }
    buff[recv_bytes] = '\0';
    return recv_bytes;
}

int send_msg(const char *buff)
{
    int sent_bytes = send(sockfd, buff, strlen(buff), 0);
    if (sent_bytes == -1)
    {
        close_connections();
        return -1;
    }
    return sent_bytes;
}

int send_arr(int *arr, int n)
{
    int sent_bytes = send(sockfd, arr, n * sizeof(int), 0);
    if (sent_bytes == -1)
    {
        close_connections();
        return -1;
    }
    return sent_bytes;
}

int send_num(int num)
{
    int sent_bytes = send(sockfd, &num, sizeof(num), 0);
    if (sent_bytes == -1)
    {
        close_connections();
        return -1;
    }
    return sent_bytes;
}

void menu_module()
{
    int arr[100], n, choice;
    printf("Enter numbers separated by space (end with Enter):\n");
    for (n = 0; n < 100; n++)
    {
        if (scanf("%d", &arr[n]) != 1)
            break;
    }
    getchar();

    printf("\nChoose an option:\n1. Search a number\n2. Sort numbers\n3. Split odd/even\n4. Exit\n");
    scanf("%d", &choice);

    send_num(n);
    send_arr(arr, n);
    send_num(choice);

    recieve_msg(buff);
    printf("Server: %s\n", buff);

    if (choice != 4)
    {
        recieve_msg(buff);
        printf("Server: %s\n", buff);
    }
}

int main()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("Connection failed");
        return -1;
    }

    while (1)
    {
        menu_module();
    }

    close_connections();
    return 0;
}