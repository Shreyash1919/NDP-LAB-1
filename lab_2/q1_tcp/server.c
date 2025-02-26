#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 2000

int server, client;
struct sockaddr_in serveraddr, clientaddr;
char buff[MAXSIZE];

void close_connections()
{
    close(client);
    close(server);
}

int receive_msg(char *buff)
{
    int recv_bytes = recv(client, buff, MAXSIZE - 1, 0);
    if (recv_bytes <= 0)
    {
        close_connections();
        return -1;
    }
    buff[recv_bytes] = '\0';
    return recv_bytes;
}

int receive_num(int *num)
{
    int recv_bytes = recv(client, num, sizeof(int), 0);
    if (recv_bytes <= 0)
    {
        close_connections();
        return -1;
    }
    return recv_bytes;
}

int receive_arr(int *arr, int n)
{
    int recv_bytes = recv(client, arr, n * sizeof(int), 0);
    if (recv_bytes <= 0)
    {
        close_connections();
        return -1;
    }
    return recv_bytes;
}

int send_msg(const char *buff)
{
    int sent_bytes = send(client, buff, strlen(buff), 0);
    if (sent_bytes == -1)
    {
        close_connections();
        return -1;
    }
    return sent_bytes;
}

void search_module(int *arr, int n)
{
    int num;
    if (receive_num(&num) == -1)
        return;

    for (int i = 0; i < n; i++)
    {
        if (arr[i] == num)
        {
            snprintf(buff, MAXSIZE, "Number found at index %d", i);
            send_msg(buff);
            return;
        }
    }
    send_msg("Number not found");
}

void sort_module(int *arr, int n)
{
    int temp;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    snprintf(buff, MAXSIZE, "Sorted array: ");
    for (int i = 0; i < n; i++)
    {
        snprintf(buff + strlen(buff), MAXSIZE - strlen(buff), "%d ", arr[i]);
    }
    send_msg(buff);
}

void split_module(int *arr, int n)
{
    char odd[MAXSIZE] = "Odd numbers: ", even[MAXSIZE] = "Even numbers: ";
    for (int i = 0; i < n; i++)
    {
        if (arr[i] % 2 == 0)
        {
            snprintf(even + strlen(even), MAXSIZE - strlen(even), "%d ", arr[i]);
        }
        else
        {
            snprintf(odd + strlen(odd), MAXSIZE - strlen(odd), "%d ", arr[i]);
        }
    }
    send_msg(odd);
    send_msg(even);
}

void menu_module()
{
    int arr[100], n, choice;
    if (receive_num(&n) == -1)
        return;
    if (receive_arr(arr, n) == -1)
        return;
    if (receive_num(&choice) == -1)
        return;

    switch (choice)
    {
    case 1:
        search_module(arr, n);
        break;
    case 2:
        sort_module(arr, n);
        break;
    case 3:
        split_module(arr, n);
        break;
    case 4:
        exit(0);
    default:
        send_msg("Invalid choice");
    }
}

int main()
{
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        perror("Socket creation failed");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("Bind failed");
        close(server);
        return -1;
    }

    if (listen(server, 1) == -1)
    {
        perror("Listen failed");
        close(server);
        return -1;
    }
    printf("Server is listening...\n");

    socklen_t len = sizeof(clientaddr);
    client = accept(server, (struct sockaddr *)&clientaddr, &len);
    if (client == -1)
    {
        perror("Accept failed");
        close(server);
        return -1;
    }
    printf("Client connected: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    while (1)
    {
        menu_module();
    }

    close_connections();
    return 0;
}