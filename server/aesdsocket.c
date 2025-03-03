/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include <stdbool.h>

#define PORT "9000"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 20000 // max internal buffer size
#define MAX_READ_SIZE 1000 // chink of data to process (read/send)
bool rx_done = false;

void sig_handler(int s)
{
    syslog(LOG_INFO, "Caught signal, exiting");
    rx_done = true;
    printf("\n server: exiting on signal \n");
    remove("/var/tmp/aesdsocketdata");
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[MAXDATASIZE];
    int status;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my
    int numbytes;
    uint rec_size = 0;
    size_t read_b = 0;
    // creating file pointer to work with files
    FILE *fptr;

    fprintf(stderr, "server: argc = %d\n", argc);

    openlog(NULL, 0, LOG_USER);

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(-1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(-1);
    }

    if (argc > 1)
    {
        int pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(-1);
        }
        else if (pid == 0)
        {
            fprintf(stderr, "server: child created \n");
        }
        else // main server
        {
            return 0;
        }
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(-1);
    }

    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction((SIGINT) , &sa, NULL) == -1) {
        perror("SIGINT");
        exit(-1);
    }
    if (sigaction((SIGTERM) , &sa, NULL) == -1) {
        perror("SIGTERM");
        exit(-1);
    }

    printf("server: waiting for connections...\n");



    while(!rx_done) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);

        syslog(LOG_INFO, "Accepted connection from %s\n", s );

        printf("server: got connection from %s\n", s);

        fptr = fopen("/var/tmp/aesdsocketdata", "a+");
        perror("fptr");

        // receiving loop
        while (true)
        {
            if ((numbytes = recv(new_fd, &buf[rec_size], MAX_READ_SIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }

            printf("server: received  %d bytes \n", numbytes);
            rec_size = rec_size + (numbytes) ;

            if (buf[rec_size-1] == '\n')
            {
                fwrite(buf , sizeof(char), rec_size, fptr);
                printf("server: saving %d bytes \n", numbytes);
                fflush(fptr);
                memset(buf, 0, sizeof(buf));
                rec_size = 0;
                fseek(fptr, 0, SEEK_SET);

                // sending loop
                while (true)
                {
                    read_b = fread( buf, sizeof( char ), MAX_READ_SIZE, fptr );

                    if (read_b == 0)
                    {
                        perror("fread");
                        break;
                    }
                    else
                    {
                        if(send(new_fd, buf, read_b, 0) == -1)
                        {
                            perror("send");
                        }
                        printf("server: sending more bytes %d... \n", (int)read_b);
                    }
                }

                syslog(LOG_INFO, "Closed connection from %s\n", s );
                printf("server: client disconnected  %s \n", s);

                close(new_fd);
                fclose(fptr);
                break;

            }
            else if (0 == numbytes)
            {
                // rx_done = true;
                syslog(LOG_INFO, "Closed connection from %s\n", s );
                printf("server: received  %d bytes \n", numbytes);
                break;
            }
            else
            {
                printf("server: received  %d bytes, waiting for more ... \n", numbytes);
            }
        }

    }
    printf("server: Shuting down \n");
    close(sockfd);
    close(new_fd);
    return 0;
}
