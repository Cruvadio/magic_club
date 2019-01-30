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
#include "strings.h"

int sem_id;
int connections[MAX_CONNECTIONS];
int connected = 0;
int sock_fd;


pthread_mutex_t mut;
pthread_cond_t cond;



void *accept_client(void *args);
void exit_handler (int signum);

struct sockaddr_in sockaddr;

void *waitForPlayers (void* args);

int main(int argc, char *argv[])
{
    GameManager game;
    thread_args args[MAX_CONNECTIONS];
    uid_t euid;
    int opt = 1;
    int port;
    int connections[MAX_CONNECTIONS] = {-1, -1};
    pthread_t threads[MAX_CONNECTIONS];
    pthread_t server_thread;

    if(argc < 2)
    {
        fprintf(stderr, "Not enough arguments\n");
        
        return 1;
    }
    
    port = atoi(argv[1]);
    euid = geteuid();
    
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
    
    while(1)
    {
        int i;
        int fd;
        
        if (game.getConnected() < MAX_CONNECTIONS)
        {
            int connected = game.getConnected();
            for (i = 0; i < MAX_CONNECTIONS; i++)
            {
                if (connections[i]  == -1)
                {
                    break;
                }
            }
            connections[i] = accept(sock_fd, NULL, 0);
            if(connections[i] == -1)
            {
                perror("Client socket has been not created\n");
                return 1;
            }
            game.players[i].setSocketFD(connections[i]);
            game.lock();
                connected++;
                game.setConnected(connected);
            game.unlock();            
            if (connected == MAX_CONNECTIONS)
                game.players[0].broadcast();
            args[i].num = i;
            args[i].fd = connections[i];
            args[i].game = &game;
            if(pthread_create(&threads[i],NULL,acceptPlayer,&(args)))
            {
                printf("Can't create thread\n");

                close(sock_fd);
                return -1;
            }
        }
        else
        {
            if(pthread_create(&server_thread, NULL, waitForPlayers, &game))
            {
                printf("Can't create thread\n");

                close(sock_fd);
                return -1;
            }
            fd = accept(sock_fd, NULL, 0);
            if (safewrite(fd, BUSY_MSG, sizeof(BUSY_MSG)))
            {
                fprintf(stderr, "Can't write message for client\n");
                close(fd);
            }
            for (int i = 0; i < MAX_CONNECTIONS; i++)
                connections[i] = -1;
            close(fd);
            pthread_cond_wait(&cond, &mut);
            continue;
        }
    }
    close(sock_fd);
    return 0;
}

void *waitForPlayers (void* args)
{    
    GameManager &game = *(GameManager*) args;
    int counter = 0;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        game.players[i].broadcast();
    }
    while(true)
    {
        while (true)
        {
            counter = 0;
            game.lock();
            for (int i = 0; i < MAX_CONNECTIONS; i++)
            {
                if (game.players[i].getMode() == READY)
                    counter++;
            }
            if (counter == MAX_CONNECTIONS)
                break;
            game.unlock();
           game. wait();
        }
        game.controlGame();
        game.gameStatus();
        for (int i = 0; i < MAX_CONNECTIONS; i++)
        {
            game.players[i].broadcast();
        }
    }
}
/*
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
        write (client_fd, FIRST_PLAYER, 
				          sizeof(FIRST_PLAYER));
        write (client_fd, WAIT, sizeof(WAIT));
        pthread_cond_wait(&cond, &mut);
        write (client_fd, FOUND, sizeof(FOUND));
    }
    else
    {
        write (client_fd, SECOND_PLAYER, 
				          sizeof(SECOND_PLAYER));
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
*/


void exit_handler (int signum)
{
    signal(signum, exit_handler);
    semctl(sem_id, 2, IPC_RMID);
    close(sock_fd);
    exit(0);
}


