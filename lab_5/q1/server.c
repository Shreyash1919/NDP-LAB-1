#include <time.h>
#include <signal.h>
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

char time_str[64];
int clients[MAXSIZE];
int c_count = 0;
char sbuff[MAXSIZE];


void *send_messages() {
    scanf("%[^\n]%*c", sbuff);
    for (int i = 0; i < MAXSIZE; i++) {
        if (clients[i] != 0) {
            send(clients[i], sbuff, sizeof(sbuff), 0);
        }
    }
    bzero(sbuff, MAXSIZE);
}

void log_message(const char *message) {
    FILE *logfile = fopen("server_logs.txt", "a");  // Open log file in append mode
    if (logfile == NULL) {
        perror("Failed to open log file");
        exit(1);
    }
    fprintf(logfile, "%s\n", message);  // Write log message
    fclose(logfile);  // Close the file after writing
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

        if (c_count == 3) {
            printf("Terminated Session\n");
            for (int i = 0; i < MAXSIZE; i++) {
                if (clients[i] != 0) {
                    close(clients[i]);
                }
            }
            signal(SIGQUIT, SIG_IGN);
            kill(0, SIGQUIT);
            pthread_cancel(thread_id);
            close(sockfd);
            exit(-1);
        }

        if ((childpid = fork()) == 0)
        {
            printf("Child process PID: %d, PPID: %d\n", getpid(), getppid());
            close(sockfd);

            while (1)
            {
                int recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
                buff[recedbytes] = '\0';
                if (strcmp(buff, ":exit") == 0 || recedbytes == -1)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                    close(newsockfd);
                    break;
                }
                printf("[%s - %s:%d] Client Message: ", get_time(), (char *)inet_ntoa((struct in_addr)clientaddr.sin_addr), (int)ntohs(clientaddr.sin_port));
                puts(buff);
                char logs[1024];
                snprintf(logs, 1024, "[%s - %s:%d] Client Message: %s", get_time(), (char *)inet_ntoa((struct in_addr)clientaddr.sin_addr), (int)ntohs(clientaddr.sin_port), buff);
                log_message(logs);
                int sentbytes = send(newsockfd, buff, sizeof(buff), 0);
                bzero(buff, MAXSIZE);

                if (sentbytes == -1)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                    close(newsockfd);
                    break;
                }
            }
        }
    }

    pthread_cancel(thread_id);

    close(sockfd);
    close(newsockfd);
}
