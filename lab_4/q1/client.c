#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define MAXSIZE 50

void receive_message(int sockfd)
{
    int recedbytes;
    char buff[MAXSIZE];
    while (1)
    {
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1)
        {
            printf("Error in receiving message\n");
            break;
        }
        if (recedbytes == 0)
        {
            break; // server closed the connection
        }
        printf("Text Received from Server: ");
        puts(buff);
    }
}

void handle_comm(int sockfd)
{
    int sentbytes;
    int recedbytes;
    char buff[MAXSIZE];
    int choice, reg, code;

    while (1)
    {
        printf("Enter data to send 1 - reg, 2 - name, 3 - course code: ");
        scanf("%d", &choice);
        sprintf(buff, "%d", choice);
        sentbytes = send(sockfd, buff, strlen(buff), 0);
        if (sentbytes == -1)
        {
            printf("Error in sending choice\n");
            break;
        }

        switch (choice)
        {
        case 1:
            printf("Enter the reg no: ");
            scanf("%d", &reg); // Corrected to use &reg
            sprintf(buff, "%d", reg);
            sentbytes = send(sockfd, buff, strlen(buff), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending reg number\n");
                break;
            }
            memset(buff, 0, sizeof(buff));

            // Receive and display the name and address of the student
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving name\n");
                break;
            }
            printf("\nName received: ");
            puts(buff);
            memset(buff, 0, sizeof(buff));

            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving address\n");
                break;
            }
            printf("\nAddress received: ");
            puts(buff);
            memset(buff, 0, sizeof(buff));

            // Receive and display the process ID (PID) of the child process handling the request
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving PID\n");
                break;
            }
            printf("PID of handling process: ");
            puts(buff);

            break;

        case 2:
            getchar(); // To clear the newline left by the previous scanf
            printf("Enter the name: ");
            fgets(buff, sizeof(buff), stdin);
            buff[strcspn(buff, "\n")] = 0; // Remove the newline character
            sentbytes = send(sockfd, buff, strlen(buff), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending name\n");
                break;
            }

            // Receive and display department, semester, section, and courses
            memset(buff, 0, sizeof(buff)); // Clear the buffer before receiving each piece of data
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving department\n");
                break;
            }
            printf("\nDepartment: ");
            puts(buff);

            memset(buff, 0, sizeof(buff)); // Clear the buffer before the next receive
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving semester\n");
                break;
            }
            printf("\nSemester: ");
            puts(buff);

            memset(buff, 0, sizeof(buff)); // Clear the buffer before the next receive
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving section\n");
                break;
            }
            printf("\nSection: ");
            puts(buff);

            // Receive and display the courses
            printf("\nCourses: ");
            for (int i = 0; i < 5; i++)
            {
                memset(buff, 0, sizeof(buff)); // Clear the buffer before receiving each course
                recedbytes = recv(sockfd, buff, sizeof(buff), 0);
                if (recedbytes == -1)
                {
                    printf("Error in receiving course name\n");
                    break;
                }
                printf("\n%s ", buff);
            }
            printf("\n");

            // Receive PID once here
            memset(buff, 0, sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving PID\n");
                break;
            }
            printf("PID of handling process: ");
            puts(buff);

            break;

        case 3:
            printf("Enter code: ");
            scanf("%d", &code); // Corrected to use address of code
            sprintf(buff, "%d", code);
            sentbytes = send(sockfd, buff, strlen(buff), 0);
            if (sentbytes == -1)
            {
                printf("Error in sending course code\n");
                break;
            }

            // Receive and display marks for the course

            printf("Marks obtained: ");
            memset(buff, 0, sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving marks\n");
                break;
            }
            puts(buff);
            printf("\n");

            memset(buff, 0, sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving marks\n");
                break;
            }
            puts(buff);
            printf("\n");

            memset(buff, 0, sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving marks\n");
                break;
            }
            puts(buff);
            printf("\n");

            memset(buff, 0, sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving marks\n");
                break;
            }
            puts(buff);
            printf("\n");

            memset(buff, 0, sizeof(buff));
            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving marks\n");
                break;
            }
            puts(buff);
            printf("\n");

            memset(buff, 0, sizeof(buff));

            recedbytes = recv(sockfd, buff, sizeof(buff), 0);
            if (recedbytes == -1)
            {
                printf("Error in receiving PID\n");
                break;
            }
            printf("PID of handling process: ");
            puts(buff);
            printf("\n");

            break;

        default:
            printf("Invalid choice, please try again.\n");
            break;
        }

       
    }
}

int main()
{
    int sockfd, retval;
    struct sockaddr_in serveraddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket Creation Error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("Connection error\n");
        close(sockfd);
        return -1;
    }

    int i = 0;

    handle_comm(sockfd);

    close(sockfd);
    return 0;
}
