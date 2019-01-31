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

void GameManager::checkBuffs()
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (players[i].getBuffs().empty()) continue;
        else
        {
            for (int j = 0; j < (int)players[i].getBuffs().size(); j++)
            {
                if (!players[i].getBuffs()[j].getTime()) 
                {
                    players[i].getBuffs().erase(players[i].getBuffs().begin() + j);
                }
                else 
                {
                    // TO DO
                    // Activation of buffs
                    // players[i].getBuffs()[j].activate();
                }
            }
        }
    }
}

void GameManager::controlGame ()
{
    bool is_increased = false;
    printf("Controlling game\n");
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        Buff buff(0, st_NONE, 0, false, true);
        if (players[i].containsBuff(buff) == -1)
            is_increased = true;
        switch(players[i].getStat().getLeftHand())
        {
            case LIGHT:
                    lightSpell(i, is_increased);
                    break;
            case DARK:
                    darkSpell(i, is_increased);
                    break;
            case FIRE:
                    fireSpell(i, is_increased);
                    break;
            case WATER:
                    waterSpell(i);
                    break;
            case EARTH:
                    earthSpell(i, is_increased);
                    break;
            case AIR:
                    airSpell(i);
                    break;
        }
        switch(players[i].getStat().getRightHand())
        {
            case LIGHT:
                    lightSpell(i, is_increased);
                    break;
            case DARK:
                    darkSpell(i, is_increased);
                    break;
            case FIRE:
                    fireSpell(i, is_increased);
                    break;
            case WATER:
                    waterSpell(i);
                    break;
            case EARTH:
                    earthSpell(i, is_increased);
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
void GameManager::lightSpell(int player_num, bool is_increased)
{
    players[player_num].getStat().addHealth(!is_increased ? LIGHT_HEALTH : LIGHT_HEALTH * 2);
    players[player_num].scaleHealth();
}

void GameManager::darkSpell(int player_num, bool is_increased)
{
    int enemy_num = player_num ? 0 : 1;
    printf("Player # %d my enemy %d\n", player_num, enemy_num);
    players[enemy_num].getStat().addHealth(!is_increased ? DARK_DAMAGE : DARK_DAMAGE * 2);
    players[enemy_num].scaleHealth();
}
void GameManager::waterSpell(int player_num)
{
    Buff regeneration(WATER_BUFF_REGENERATION_DURATION, HEALTH, WATER_BUFF_REGENERATION_HEALTH, true);
    Buff  froze(WATER_BUFF_FROST);

    players[player_num].addBuff(regeneration);
    players[!player_num].addBuff(froze);
}
void GameManager::fireSpell(int player_num, bool is_increased)
{
    Buff burning(!is_increased ? FIRE_BUFF_DURATION : FIRE_BUFF_DURATION * 2, DAMAGE, FIRE_BUFF_DAMAGE, true);
    players[!player_num].addBuff(burning);
    players[!player_num].getStat().addHealth(!is_increased ? FIRE_DAMAGE : FIRE_DAMAGE * 2);
}
void GameManager::earthSpell(int player_num, bool is_increased)
{
    players[player_num].getStat().addShield(!is_increased ? EARTH_SHIELD : EARTH_SHIELD * 2);
}
void GameManager::airSpell(int player_num)
{
    Buff increase(AIR_DURATION, st_NONE, 0, false, true);
    players[player_num].addBuff(increase);
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


