#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 256

// Function to compare integers for ascending order
int compare_integers(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// Function to compare characters for descending order
int compare_characters(const void *a, const void *b)
{
    return (*(char *)b - *(char *)a);
}

// Function to sort digits and characters from the alphanumeric string
void sort_string(const char *input, char *sorted_chars, int *sorted_digits, int *num_digits, int *num_chars)
{
    *num_digits = 0;
    *num_chars = 0;

    // Separate digits and characters
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (isdigit(input[i]))
        {
            sorted_digits[(*num_digits)++] = input[i] - '0'; // Convert char to int
        }
        else if (isalpha(input[i]))
        {
            sorted_chars[(*num_chars)++] = input[i];
        }
    }

    // Sort digits in ascending order
    qsort(sorted_digits, *num_digits, sizeof(int), compare_integers);

    // Sort characters in descending order
    qsort(sorted_chars, *num_chars, sizeof(char), compare_characters);
}

void handle_client(int client_sock)
{
    char buffer[BUFFER_SIZE];
    int bytes_received;
    char sorted_chars[128];
    int sorted_digits[128];
    int num_digits, num_chars;

    while (1)
    {

        // Receive the alphanumeric string from the client
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                printf("Client disconnected.\n");
            }
            else
            {
                perror("recv");
            }
            return;
        }
        printf("Received string from client: %s\n", buffer);

        // Sort the digits and characters
        sort_string(buffer, sorted_chars, sorted_digits, &num_digits, &num_chars);

        // Display process ID and PPID
        printf("Parent Process PID: %d, PPID: %d\n", getpid(), getppid());

        // Send the sorted digits (ascending order) from the child process
        if (fork() == 0)
        {
            // Child process
            char result[BUFFER_SIZE];
            sprintf(result, "Sorted Digits (Ascending): ");
            for (int i = 0; i < num_digits; i++)
            {
                char temp[10];
                sprintf(temp, "%d", sorted_digits[i]);
                strcat(result, temp);
                strcat(result, " ");
            }
            strcat(result, "\nChild PID: ");
            sprintf(buffer, "%d", getpid());
            strcat(result, buffer);
            send(client_sock, result, strlen(result), 0);
            exit(0);
        }
        else
        {
            // Parent process
            char result[BUFFER_SIZE];
            sprintf(result, "Sorted Characters (Descending): ");
            for (int i = 0; i < num_chars; i++)
            {
                char temp[2] = {sorted_chars[i], '\0'};
                strcat(result, temp);
                strcat(result, " ");
            }
            strcat(result, "\nParent PID: ");
            sprintf(buffer, "%d", getpid());
            strcat(result, buffer);
            send(client_sock, result, strlen(result), 0);
        }
    }
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1)
    {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock == -1)
        {
            perror("accept");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        // Handle the client communication
        handle_client(client_sock);
    }

    // Clean up
    close(client_sock);
    close(server_sock);

    return 0;
}
