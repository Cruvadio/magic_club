#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "game.h"


Player::Player() : stat(), socket_fd(-1), buffs(), mode(WAITING), is_alive(true)
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
/*
void* Player::acceptPlayer(void* args)
{
	this->socket_fd = accept(socket_fd, NULL, 0);
	if(this->socket_fd == -1)
    {
		perror("Client socket has not created\n");
		return 1;
    }
    return this->socket_fd;
}
*/
void Player::diconnectFromServer()
{
	
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


