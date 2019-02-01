#pragma once

#ifndef _PLAYER_H
#define _PLAYER_H

#include <vector>
#include <pthread.h>
#include "game.h"


class GameManager;
class Player;
enum number_t
{
    FIRST = 1,
    SECOND
};

enum status_t
{
    WAITING,
    READY
};

enum magic_t
{
    m_NONE,
    LIGHT,
    DARK,
    FIRE,
    WATER,
    EARTH,
    AIR
};

enum stat_t
{
	st_NONE,
	HEALTH,
	SHIELD,
	DAMAGE
};


class Stats
{
	int health;
	int shield;
    friend class Player;
	magic_t left_hand;
	magic_t right_hand;
			
	public:
	    Stats(): health(100), shield(0), left_hand(m_NONE), right_hand(m_NONE){}
		int getHealth() {	return health;	}
		int getShield() {	return shield;	}
		void setHealth(int health);
		void setShield(int shield);
		void addHealth(int health);
		void addShield(int shield);

		magic_t getLeftHand();
		magic_t getRightHand();
};

class Buff
{
    friend class Player;
	int time;
	stat_t st;
    int value;
	bool is_active;
    bool is_increasing;
	
	public:
		Buff(): time(0), st(st_NONE), value(0), is_active(false), is_increasing(false) {}
		Buff(const Buff &buff);
		Buff(int time, stat_t st =st_NONE,int value = 0, bool is_active = false, bool is_increasing = false);
		int getTime();
		void tick();

        bool operator==(const Buff &buff);
};

void* acceptPlayer(void* args);

class Player
{
    number_t number;
    Stats stat;
    int socket_fd;
    std::vector<Buff> buffs;
    status_t status;
    bool is_alive;

    pthread_cond_t cond;
    pthread_mutex_t mut;
   
    int checkComand (char *str);
       public:
	    Player();

        void activateBuff(int i);
	    
        void lock();
        void unlock();
        void wait();
        void broadcast();

        int  containsBuff(Buff buff);

        std::vector<Buff> & getBuffs ();
        void addBuff (Buff buff);
        void removeBuff(Buff buff);
	    //
	    // SERVER METHODS
	    //
        
        void setSocketFD(int sock_fd);
	    int getSocketFD() {	return socket_fd;   }
	    void* acceptPlayer(GameManager& game, int num, int fd);
	    void diconnectFromServer (GameManager &game);
        status_t getMode() { return status; }
        const char* handToString(int hand);
        
        void setStatus(status_t st);
        void scaleHealth ();
        Stats& getStat();

	    //
	    // DESTRUCTOR
	    //
	    ~Player();

};

#endif
