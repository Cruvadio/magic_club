#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "strings.h"
#include "game.h"


Player::Player() : stat(), socket_fd(-1), buffs(), status(WAITING), is_alive(true)
{
    number = FIRST;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mut, NULL);
}

Player::~Player()
{
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mut);
}

void* Player::acceptPlayer(void* args)
{
    socket_fd = *(int*) args;
    char buf[1024];
    
    if (safewrite(socket_fd, SERVER_NAME, strlen(SERVER_NAME) * sizeof(char)))
    {
         fprintf(stderr, "Can't write message for client\n");
         close(socket_fd);
         return NULL;
    }
    if (!(number == FIRST) && GameManager::getGameManager().getConnected() < MAX_CONNECTIONS)
    {
        write (socket_fd, FIRST_PLAYER,sizeof(FIRST_PLAYER));
        write (socket_fd, WAIT, sizeof(WAIT));
        wait();
        write (socket_fd, FOUND, sizeof(FOUND));
    }
    else
    {
        write (socket_fd, SECOND_PLAYER, sizeof(SECOND_PLAYER));
    }
    while (1)
    {
        int err;
        wait();
        write(socket_fd, PROMT, sizeof(PROMT));
        do
        {
            err = readstr(socket_fd, buf, 1024);
            if (err == -1)
            {
                diconnectFromServer();
                return NULL;
            }
            lock();
            err = checkComand(buf);
            unlock();
            if (err)
            {
                write(socket_fd, UNKNOWN_CMD, sizeof(UNKNOWN_CMD));
                continue;
            }
            else
                break;
        }while (true);
       
        if (GameManager::getGameManager().getConnected() < MAX_CONNECTIONS)
        { 
            write(socket_fd, SORRY, sizeof(SORRY));
            diconnectFromServer();
            return NULL;
        }

        lock();
            status = READY;
            GameManager::getGameManager().broadcast();
        unlock();
    }

}

int Player::checkComand(char* str)
{
    std::string tmp(str);
    std::string lh, rh; // Left hand and right hand strings
    if (tmp.find("cast") == std::string::npos)
        return -1;
    else
    {
        lh = tmp.substr(strlen("cast "), tmp.find(" ", strlen("cast ")));
        rh = tmp.substr(strlen("cast ") + lh.length());
        
        if (!lh.compare("light")) stat.left_hand = LIGHT;
        else if (!lh.compare("dark")) stat.left_hand = DARK;
        else if (!lh.compare("fire")) stat.left_hand = FIRE;
        else if (!lh.compare("water")) stat.left_hand = WATER;
        else if (!lh.compare("earth")) stat.left_hand = EARTH;
        else if (!lh.compare("air")) stat.left_hand = AIR;
        else return -1;

        if (!rh.compare("light")) stat.left_hand = LIGHT;
        else if (!rh.compare("dark")) stat.right_hand = DARK;
        else if (!rh.compare("fire")) stat.right_hand = FIRE;
        else if (!rh.compare("water")) stat.right_hand = WATER;
        else if (!rh.compare("earth")) stat.right_hand = EARTH;
        else if (!rh.compare("air")) stat.right_hand = AIR;
        else return -1;
    }
}

void Player::diconnectFromServer()
{
    printf ("Client has disconnected\n");
     GameManager::getGameManager().lock();
    {
        int connected = GameManager::getGameManager().getConnected();
        socket_fd = -1;
        connected--;
        GameManager::getGameManager().setConnected(connected);
        GameManager::getGameManager().broadcast();
    }
    GameManager::getGameManager().lock();
    close(socket_fd);
}

void Player::lock()
{
    pthread_mutex_lock(&mut);
}

void Player::unlock()
{
    pthread_mutex_unlock(&mut);
}

void Player::wait()
{
    pthread_cond_wait(&cond, &mut);
}

void Player::broadcast()
{
    pthread_cond_broadcast(&cond);
}

const char* Player::handToString (int hand)
{
    if (hand)
    {
        switch (stat.right_hand)
        {
            case LIGHT:
                return "Light magic";
                break;
            case DARK:
                return "Dark magic";
                break;
            case FIRE:
                return "Fireball";
                break;
            case WATER:
                return "Water magic";
                break;
            case EARTH:
                return "Earth magic";
                break;
            case AIR:
                return "Ait magic";
                break;
            default:
                return "Unknown magic";
                break;
        }
    }
    else
    {
        switch (stat.left_hand)
        {
            case LIGHT:
                return "Light magic";
                break;
            case DARK:
                return "Dark magic";
                break;
            case FIRE:
                return "Fireball";
                break;
            case WATER:
                return "Water magic";
                break;
            case EARTH:
                return "Earth magic";
                break;
            case AIR:
                return "Ait magic";
                break;
            default:
                return "Unknown magic";
                break;
        }
    }
}

void Stats::setShield(int shield)
{
	this->shield = shield;
}

void Player::scaleHealth()
{
	if (this->stat.getHealth() > MAX_HEALTH)
		this->stat.setHealth(MAX_HEALTH);
	else if (this->stat.getHealth() <= MIN_HEALTH)
	{
		this->stat.setHealth(MIN_HEALTH);
		this->is_alive = false;
	}	
}

void Stats::setHealth(int health)
{
	this->health = health;
}

void Stats::addHealth(int health)
{
	this->health += health;
}

void Stats::addShield (int shield)
{
	this->shield += shield;
}


