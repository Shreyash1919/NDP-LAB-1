#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#define MAXSIZE 90

int sockfd, newsockfd, retval;
socklen_t actuallen;
struct sockaddr_in serveraddr, clientaddr;

struct course
{
    char name[50];
    int code;
    int marks;
} courses[5];

struct student
{
    char name[50];
    int reg;
    char address[100];
    char dept[30];
    int sem;
    char section;
    struct course c[5];
} students[5];

// Sample dataset of courses
struct course courses[5] = {
    {"Computer Science", 101, 0},
    {"Mathematics", 102, 0},
    {"Physics", 103, 0},
    {"Chemistry", 104, 0},
    {"Biology", 105, 0}};

// Sample dataset of students with marks
struct student students[5] = {
    {"Alice Johnson", 1001, "123 Maple Street", "Computer Science", 1, 'A', {{"Computer Science", 101, 88}, {"Mathematics", 102, 92}, {"Physics", 103, 85}, {"Chemistry", 104, 90}, {"Biology", 105, 95}}},

    {"Bob Smith", 1002, "456 Oak Avenue", "Mechanical Engineering", 2, 'B', {{"Computer Science", 101, 78}, {"Mathematics", 102, 82}, {"Physics", 103, 80}, {"Chemistry", 104, 86}, {"Biology", 105, 91}}},

    {"Charlie Brown", 1003, "789 Pine Road", "Electrical Engineering", 3, 'C', {{"Computer Science", 101, 95}, {"Mathematics", 102, 87}, {"Physics", 103, 88}, {"Chemistry", 104, 92}, {"Biology", 105, 89}}},

    {"David Lee", 1004, "101 Birch Lane", "Civil Engineering", 4, 'D', {{"Computer Science", 101, 72}, {"Mathematics", 102, 75}, {"Physics", 103, 70}, {"Chemistry", 104, 78}, {"Biology", 105, 82}}},

    {"Eve White", 1005, "202 Cedar Blvd", "Chemical Engineering", 5, 'E', {{"Computer Science", 101, 85}, {"Mathematics", 102, 90}, {"Physics", 103, 83}, {"Chemistry", 104, 88}, {"Biology", 105, 91}}}};

int send_msg(const char *buff, int newsockfd)
{
    int sentbytes;
    sentbytes = send(newsockfd, buff, strlen(buff), 0);
    if (sentbytes == -1)
    {
        printf("Error in sending message\n");
        return -1;
    }
    return 0;
}

void child1_process(int reg)
{
    // printf("Child Process 1 Starting!\n");

    int i;
    for (i = 0; i < 5; i++)
    {
        if (students[i].reg == reg)
        {
            send_msg(students[i].name, newsockfd);
            sleep(1);
            send_msg(students[i].address, newsockfd);
            sleep(1);
            char pid_str[MAXSIZE];
            sprintf(pid_str, "%d", getpid());
            send_msg(pid_str, newsockfd);
            return;
        }
    }
    sleep(1);

    send_msg("Not found", newsockfd);
    // printf("Child Process 1 Terminating!!\n");
    return;
}

void child2_process(char name[])
{
    printf("Child Process 2!\n");

    int i, j;
    for (i = 0; i < 5; i++)
    {
        if (strcmp(students[i].name, name) == 0)
        {
            send_msg(students[i].dept, newsockfd);
            sleep(1);

            char sem_str[MAXSIZE];
            sprintf(sem_str, "%d", students[i].sem);
            send_msg(sem_str, newsockfd);
            sleep(1);

            send_msg(&students[i].section, newsockfd);
            sleep(1);
            for (j = 0; j < 5; j++)
            {
                send_msg(students[i].c[j].name, newsockfd);
                sleep(1);
            }
            char pid_str[MAXSIZE];
            sprintf(pid_str, "%d", getpid());
            send_msg(pid_str, newsockfd);
            return;
        }
    }
    send_msg("Not found", newsockfd);
    // printf("Child Process 2 Terminating!!\n");
    return;
}

void child3_process(int code)
{
    printf("Child Process 3!\n");

    int i, j;
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (students[i].c[j].code == code)
            {
                char marks_str[MAXSIZE];
                sprintf(marks_str, "%d", students[i].c[j].marks);
                send_msg(marks_str, newsockfd);
            }
        }
    }
    char pid_str[MAXSIZE];
    sprintf(pid_str, "%d", getpid());
    send_msg(pid_str, newsockfd);
    // printf("Child Process 3 Terminating!!\n");
    return;
}

void handle_client(int newsockfd)
{
    int recedbytes;
    char buff[MAXSIZE];
    while (1)
    {
        memset(buff, 0, sizeof(buff));

        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1)
        {
            printf("Error in receiving msg\n");
            break;
        }
        if (recedbytes == 0)
        {
            break;
        }

        int request_type = atoi(buff);
        printf("Choice Received from Client: %d\n", request_type);
        if (request_type == 1)
        {
            int reg;
            memset(buff, 0, sizeof(buff));

            recv(newsockfd, &buff, sizeof(buff), 0);
            reg = atoi(buff);
            printf("Reg No. Recieved: %d\n", reg);
            if (fork() == 0)
            {
                child1_process(reg);

                exit(0);
            }
        }
        else if (request_type == 2)
        {
            char name[50];
            memset(name, 0, sizeof(name)); // Clear the buffer to avoid old data issues

            // Receive the string from the client
            int recedbytes = recv(newsockfd, name, sizeof(name) - 1, 0); // Reserve space for null terminator

            if (recedbytes == -1)
            {
                printf("Error receiving name.\n");
                return;
            }
            // Ensure the received string is null-terminated
            name[sizeof(name) - 1] = '\0'; // Forcefully null-terminate it

            printf("Name Received: ");
            puts(name);
            printf("\n");

            if (fork() == 0)
            {
                child2_process(name);

                exit(0);
            }
        }

        else if (request_type == 3)
        {
            int code;
            memset(buff, 0, sizeof(buff));

            recv(newsockfd, buff, sizeof(buff), 0);
            code = atoi(buff);
            if (fork() == 0)
            {
                child3_process(code);

                exit(0);
            }
        }
    }
    close(newsockfd);
}

// Main function
int main()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        perror("Binding error");
        close(sockfd);
        return -1;
    }

    retval = listen(sockfd, 1);
    printf("Server is listening on port 3388...\n");
    if (retval == -1)
    {
        perror("Listen error");
        close(sockfd);
        return -1;
    }

    actuallen = sizeof(clientaddr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

        if (newsockfd == -1)
        {
            printf("Accept error\n");
            close(sockfd);
            return -1;
        }
        printf("Connection Accepted!!\n");
        // printf("Connection accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port)); // Displaying client info

        if (fork() == 0)
        {
            close(sockfd);
            printf("Handling Client!!\n");
            handle_client(newsockfd);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
