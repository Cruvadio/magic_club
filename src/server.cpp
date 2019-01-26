#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sem.h>
#include <signal.h>
#include <netdb.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <errno.h>
#include <limits.h>
#include "game.h"

int sem_id;
int connections[MAX_CONNECTIONS];
int connected = 0;
int sock_fd;

pthread_mutex_t mut;
pthread_cond_t cond;
thread_args args[MAX_CONNECTIONS];

pthread_mutex_t client_mut;
pthread_cond_t client_cond;
pthread_t threads[MAX_CONNECTIONS];

void *accept_client(void *args);
void exit_handler (int signum);

struct sockaddr_in sockaddr;

int main(int argc, char *argv[])
{
    uid_t euid;
    int opt = 1;
    int port;
    int i;
    if(argc < 2)
    {
        fprintf(stderr, "Not enough arguments\n");
        
        return 1;
    }
    
    port = atoi(argv[1]);
    euid = geteuid();
    
    pthread_mutex_init(&mut,NULL);
     pthread_mutex_init(&client_mut,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_cond_init(&client_cond,NULL);
    
    if((port > USHRT_MAX) || (port < 0) || (euid && port < 1024))
    {
        fprintf(stderr, "Invalid port number - %d\n", port);
        return 1;
    }
    
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        perror("Socket has not created\n");
        return 1;
    }
    
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    signal(SIGINT, exit_handler);
    signal(SIGTERM, exit_handler);
    
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(sock_fd, (struct sockaddr *) & sockaddr, sizeof(sockaddr)))
    {
        perror("Can`t bind address\n");
        return 1;
    }
    
    if(listen(sock_fd, 5))
    {
        perror("Can`t listen socket\n");
        return 1;
    }
    for(i = 0; i < MAX_CONNECTIONS; i++)
    {
        args[i].fd = -1;
    }
    while(1)
    {
        int i;
        int fd;
        pthread_mutex_unlock(&mut);
        
        if (connected < MAX_CONNECTIONS)
        {
            for (i = 0; i < MAX_CONNECTIONS; i++)
            {
                if (args[i].fd == -1)
                {
                    break;
                }
            }
            
            args[i].fd = accept(sock_fd, NULL, 0);
            if(args[i].fd == -1)
            {
                perror("Client socket has not created\n");
                return 1;
            }
            pthread_mutex_lock(&mut);
                connected++;
            pthread_mutex_unlock(&mut);
            
            if (connected == MAX_CONNECTIONS)
                pthread_cond_broadcast(&cond);

            args[i].st.health = MAX_HEALTH;
            args[i].st.shield = MIN_HEALTH;
            args[i].num = i;


            if(pthread_create(&threads[i],NULL,accept_client,&args[i]))
            {
                printf("Can't create thread\n");

                close(sock_fd);
                return -1;
            }
        }
        else
        {
            fd = accept(sock_fd, NULL, 0);
            if (safewrite(fd, BUSY_MSG, sizeof(BUSY_MSG)))
            {
                fprintf(stderr, "Can't write message for client\n");
                close(fd);
            }
            close(fd);
            pthread_cond_wait(&cond, &mut);
            continue;
        }
    }
    close(sock_fd);
    return 0;
}

void* accept_client(void* args)
{
    thread_args* arg = (thread_args*) args;
    int client_fd = arg->fd;
    char buf[1024];
    
    if (safewrite(client_fd, SERVER_NAME, strlen(SERVER_NAME) * sizeof(char)))
    {
         fprintf(stderr, "Can't write message for client\n");
         close(client_fd);
         return NULL;
    }
    if (!(arg->num) && connected < MAX_CONNECTIONS)
    {
        write (client_fd, "You are the FIRST player\n\r", 
				sizeof("You are the FIRST player\n\r"));
        write (client_fd, WAIT, sizeof(WAIT));
        pthread_cond_wait(&cond, &mut);
        write (client_fd, FOUND, sizeof(FOUND));
    }
    else
    {
        write (client_fd, "You are the SECOND player\n\r", 
				sizeof("You are the SECOND player\n\r"));
    }
    while (1)
    {
        int err;

        game_status(client_fd);
        write(client_fd, PROMT, sizeof(PROMT));
        err = readstr(client_fd, buf, 1024);
        if (err == -1)
        {
            printf ("Client has disconnected\n");
            pthread_mutex_lock(&mut);
                arg->fd = -1;
                connected--;
                pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mut);
            close(client_fd);
            return NULL;
        }

        write(client_fd, "Unknown command!\n\r", sizeof("Unknown command!\n\r"));

        if (connected < MAX_CONNECTIONS)
        { 
            write(client_fd, SORRY, sizeof(SORRY));
            printf ("Client has disconnected\n");
            pthread_mutex_lock(&mut);
                arg->fd = -1;
                connected--;
                pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mut);
            close(client_fd);
            return NULL;
        }
    }
}

void exit_handler (int signum)
{
    signal(signum, exit_handler);
    semctl(sem_id, 2, IPC_RMID);
    close(sock_fd);
    exit(0);
}


