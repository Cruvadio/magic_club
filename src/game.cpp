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
    for (int j = 0; j < MAX_CONNECTIONS; j++)
    {
        fd = players[j].getSocketFD();

        for (i = 0; i < MAX_CONNECTIONS; i++)
        {
            if (!i)
                sprintf(str, "First player status:\n\r");
            else
                sprintf(str, "Second player status:\n\r");
            write(fd, str, strlen(str));
            sprintf(str, "Health: %d\n\r", players[i].getStat().getHealth());
            write(fd, str, strlen(str));
            sprintf(str, "Shield: %d\n\r", players[i].getStat().getShield());
            write(fd, str, strlen(str));
            if (players[i].getStat().getLeftHand() != m_NONE)
            {
                strcpy(str, players[i].handToString(0));
                strcat(str, "\n\r");
                write(fd, str, strlen(str)); 
            }
            if (players[i].getStat().getRightHand() != m_NONE)
            {
                strcpy(str, players[i].handToString(1));
                strcat(str, "\n\r");
                write(fd, str, strlen(str)); 
            }
        }
        players[j].setStatus(WAITING);
    }
}

GameManager::GameManager() : socket_fd(-1),connected(0),players(MAX_CONNECTIONS)
{
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mut, NULL);
}

void GameManager::controlGame ()
{
    printf("Controlling game\n");
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        switch(players[i].getStat().getLeftHand())
        {
            case LIGHT:
                    lightSpell(i);
                    break;
            case DARK:
                    darkSpell(i);
                    break;
            case FIRE:
                    fireSpell(i);
                    break;
            case WATER:
                    waterSpell(i);
                    break;
            case EARTH:
                    earthSpell(i);
                    break;
            case AIR:
                    airSpell(i);
                    break;
        }
        switch(players[i].getStat().getRightHand())
        {
            case LIGHT:
                    lightSpell(i);
                    break;
            case DARK:
                    darkSpell(i);
                    break;
            case FIRE:
                    fireSpell(i);
                    break;
            case WATER:
                    waterSpell(i);
                    break;
            case EARTH:
                    earthSpell(i);
                    break;
            case AIR:
                    airSpell(i);
                    break;
        }

        if (!players[i].getStat().getHealth())
        {
            players[i].diconnectFromServer(*this);
            pthread_exit(NULL);
        }
    }
}

void GameManager::setConnected(int connected)
{
    this->connected = connected;
}

void GameManager::lightSpell(int player_num)
{
    players[player_num].getStat().addHealth(LIGHT_HEALTH);
    players[player_num].scaleHealth();
}

void GameManager::darkSpell(int player_num)
{
    players[!player_num].getStat().addHealth(DARK_DAMAGE);
    players[!player_num].scaleHealth();
}
void GameManager::waterSpell(int player_num)
{

}
void GameManager::fireSpell(int player_num)
{
    players[!player_num].getStat().addHealth(FIRE_DAMAGE);
}
void GameManager::earthSpell(int player_num)
{
    players[player_num].getStat().addShield(EARTH_SHIELD);
}
void GameManager::airSpell(int player_num)
{

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


