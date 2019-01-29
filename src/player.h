#pragma once

#ifndef _PLAYER_H
#define _PLAYER_H


#include "game.h"
#include <vector>
#include <pthread.h>
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
	int time;
	stat_t st;
	bool is_active;
	
	public:
		Buff(): time(0), st(st_NONE), is_active(false) {}
		Buff(Buff &buff);
		Buff(int time, stat_t st =st_NONE, bool is_active = false);
		int getTime();
		void tick();
		void activate();
};

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
	    
        void lock();
        void unlock();
        void wait();
        void broadcast();

        
	    //
	    // SERVER METHODS
	    //
        
        void setSocketFD(int sock_fd);
	    int getSocketFD() {	return socket_fd;   }
	    void* acceptPlayer(void* args);
	    void diconnectFromServer ();
        status_t getMode() { return status; }
        const char* handToString(int hand);

        void scaleHealth ();

	    //
	    //  SPELLS
	    //

	    void lightSpell();
	    void darkSpell();
	    void fireSpell();
	    void waterSpell();
	    void earthSpell();
	    void airSpell();
	    //
	    // DESTRUCTOR
	    //
	    ~Player();

};

#endif
