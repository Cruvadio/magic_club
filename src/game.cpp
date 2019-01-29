#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include <pthread.h>
#include "player.h"


void GameManager::gameStatus()
{
    int fd;
    int i;
    char str[1024];
    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        fd = game.players[i].getSocketFD();
        if (!i)
            sprintf(str, "First player status:\n\r");
        else
            sprintf(str, "Second player status:\n\r");
        write(fd, str, strlen(str));
        sprintf(str, "Health: %d\n\r", game.players[i].getHealth());
        write(fd, str, strlen(str));
        sprintf(str, "Shield: %d\n\r", game.players[i].getShield());
        write(fd, str, strlen(str));
        if (game.players[i].getLeftHand() != m_NONE)
        {
            strcpy(str, game.players[i].handToString(0));
            strcat(str, "\n\r");
            write(fd, str, strlen(str)); 
        }
        if (game.players[i].getRightHand() != m_NONE)
        {
            strcpy(str, game.players[i].handToString(1));
            strcat(str, "\n\r");
            write(fd, str, strlen(str)); 
        }
    }
}

GameManager::GameManager() : socket_fd(-1), players(), connected(0)
{
    game = *this;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mut, NULL);
}

GameManager::~GameManager()
{
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mut);
}
void GameManager::lock()
{
    pthread_mutex_lock(&mut);
}

void GameManager::unlock()
{
    pthread_mutex_unlock(&mut);
}

void GameManager::wait()
{
    pthread_cond_wait(&cond, &mut);
}

void GameManager::broadcast()
{
    pthread_cond_broadcast(&cond);
}

void * GameManager::waitForPlayers (void* args)
{
    int counter = 0;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        players[i].broadcast();
    }
    while(true)
    {
        while (true)
        {
            counter = 0;
            lock();
            for (int i = 0; i < MAX_CONNECTIONS; i++)
            {
                if (game.players[i].getMode() == READY)
                    counter++;
            }
            if (counter == MAX_CONNECTIONS)
                break;
            unlock();
            wait();
        }
        gameStatus();
        for (int i = 0; i < MAX_CONNECTIONS; i++)
        {
            players[i].broadcast();
        }
    }
}
