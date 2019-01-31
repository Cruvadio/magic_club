#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include <iostream>
#include "strings.h"
#include "game.h"


Player::Player() : stat(), socket_fd(-1),/* buffs(),*/ status(WAITING), is_alive(true)
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

void Player::setSocketFD(int sock_fd)
{
    socket_fd = sock_fd;
}

void* Player::acceptPlayer (GameManager &game, int num, int sock_fd)
{
    char buf[1024];
    bool firstTime = true;
    socket_fd = sock_fd;
    if (safewrite(socket_fd, SERVER_NAME, strlen(SERVER_NAME) * sizeof(char)))
    {
         fprintf(stderr, "Can't write message for client\n");
         close(socket_fd);
         return NULL;
    }
    if (!(num) && game.getConnected() < MAX_CONNECTIONS)
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
        if (!firstTime)
        wait();
        do
        {            
            write(socket_fd, PROMT, sizeof(PROMT));

            err = readstr(socket_fd, buf, 1024);
            if (err == -1)
            {
                diconnectFromServer(game);
                return NULL;
            }
            err = checkComand(buf);
            if (err)
            {
                write(socket_fd, UNKNOWN_CMD, sizeof(UNKNOWN_CMD));
                continue;
            }
            else
            {
                printf("Correct command\n");
                break;
            }
        }while (true);
       
        if (game.getConnected() < MAX_CONNECTIONS)
        { 
            write(socket_fd, SORRY, sizeof(SORRY));
            diconnectFromServer(game);
            return NULL;
        }

        status = READY;
        printf("Player # %d broadcasting main\n", num);
        game.unlock();
        firstTime = false;
    }
}

void Player::setStatus(status_t st)
{
    status = st;
}
void* acceptPlayer(void* args)
{
    thread_args *arg = (thread_args*) args;
    GameManager &game = *(arg->game);
    int num = arg->num;

    printf("Player # %d, socket file descryptor = %d\n", num, arg->fd);
    return game.players[num].acceptPlayer(game, num, arg->fd); 
}

int Player::checkComand(char* str)
{
    char *(tokens[3]);
    char *token = strtok(str, " \n\r");
    int i = 0;
    std::string tmp(str);
    std::string lh, rh; // Left hand and right hand strings
    
    while (token != NULL && i < 3)
    {
        tokens[i] = token;
        token = strtok(NULL, " \n\r");
        i++;
    }

    if (i == 3)
    {
        lh = tokens[1];
        rh = tokens[2];
    }
    if (strcmp(tokens[0], "cast"))
        return -1;
    else
    {
        printf("Is cast\n");
        
        std::cout << "First word: " << lh <<std::endl <<"Second word: " << rh << std::endl;

        if (!strcmp(tokens[1], "light")) stat.left_hand = LIGHT;
        else if (!strcmp(tokens[1], "dark")) stat.left_hand = DARK;
        else if (!strcmp(tokens[1], "fire")) stat.left_hand = FIRE;
        else if (!strcmp(tokens[1], "water")) stat.left_hand = WATER;
        else if (!strcmp(tokens[1], "earth")) stat.left_hand = EARTH;
        else if (!strcmp(tokens[1], "air")) stat.left_hand = AIR;
        else return -1;

        if (!strcmp(tokens[2], "light")) stat.right_hand = LIGHT;
        else if (!strcmp(tokens[2], "dark")) stat.right_hand = DARK;
        else if (!strcmp(tokens[2], "fire")) stat.right_hand = FIRE;
        else if (!strcmp(tokens[2], "water")) stat.right_hand = WATER;
        else if (!strcmp(tokens[2], "earth")) stat.right_hand = EARTH;
        else if (!strcmp(tokens[2], "air")) stat.right_hand = AIR;
        else return -1;

        
    }
    return 0;
}

void Player::diconnectFromServer(GameManager &game)
{
    printf ("Client has disconnected\n");
     game.lock();
    {
        int connected = game.getConnected();
        socket_fd = -1;
        connected--;
        game.setConnected(connected);
        game.broadcast();
    }
    game.unlock();
    close(socket_fd);
}

Stats& Player::getStat()
{
    return stat;
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
                return "Air magic";
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
                return "Air magic";
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

magic_t Stats::getLeftHand ()
{
    return left_hand;
}

magic_t Stats::getRightHand()
{
    return right_hand;
}

void Stats::setHealth(int health)
{
	this->health = health;
}

void Stats::addHealth(int health)
{
    if (health < 0 && shield > 0)
    { 
        int difference;
        difference = shield + health;
        if (shield)
        {
            shield+= health;
            if (shield < 0)
                shield = 0;
        }
        if (difference < 0)
            this->health += difference;
    }
    else     
	    this->health += health;
}

void Stats::addShield (int shield)
{
	this->shield += shield;
}


