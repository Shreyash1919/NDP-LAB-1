#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define MAX_DB_SIZE 100

typedef struct
{
    char domain[50];
    char ip[50];
} DNSRecord;

char time_str[64];


char *get_time()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(time_str, sizeof(time_str), "%c", tm);
    return time_str;
}

int load_dns_database(DNSRecord *db)
{
    FILE *file = fopen("database.txt", "r");
    if (file == NULL)
    {
        perror("Could not open database.txt");
        return -1;
    }

    int i = 0;
    while (fscanf(file, "%s %s", db[i].domain, db[i].ip) != EOF && i < MAX_DB_SIZE)
    {
        i++;
    }

    fclose(file);
    return i;
}

const char *find_ip(const DNSRecord *db, int db_size, const char *domain)
{
    for (int i = 0; i < db_size; i++)
    {
        if (strcmp(db[i].domain, domain) == 0)
        {
            return db[i].ip;
        }
    }
    return NULL;
}

int main()
{
    pid_t childpid;

    int server_fd, new_socket, valread, sentbytes;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE] = {0};

    DNSRecord db[MAX_DB_SIZE];
    int db_size = load_dns_database(db);
    if (db_size < 0)
    {
        exit(EXIT_FAILURE);
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int true = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server is running on port %d...\n", PORT);
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if ((childpid = fork()) == 0)
        {
            close(server_fd);
            while (1)
            {
                valread = read(new_socket, buffer, MAX_BUFFER_SIZE);
                buffer[valread] = '\0';
                if (strcmp(buffer, ":exit") == 0 || valread == -1)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(new_socket);
                    break;
                }
                printf("[%s - %s:%d] Client Message: ", get_time(), (char *)inet_ntoa((struct in_addr)client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
                printf("Received domain request - %s\n", buffer);

                const char *ip = find_ip(db, db_size, buffer);
                if (ip != NULL)
                {
                    sentbytes = send(new_socket, ip, strlen(ip), 0);
                }
                else
                {
                    const char *not_found = "Domain not found";
                    sentbytes = send(new_socket, not_found, strlen(not_found), 0);
                }

                bzero(buffer, MAX_BUFFER_SIZE);

                if (sentbytes == -1)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(new_socket);
                    break;
                }
            }
        }
    }

    return 0;
}
