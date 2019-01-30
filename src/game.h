#pragma once

#ifndef _GAME_H

#define _GAME_H

#include <pthread.h>
#include "/home/cruvadio/src/magic_club/src/player.h"

#define MAX_CONNECTIONS 2
#define MAX_HEALTH 100
#define MIN_HEALTH 0

#define LIGHT_HEALTH 15
#define DARK_DAMAGE -15
#define FIRE_DAMAGE -10
#define FIRE_BUFF_DAMAGE -5
#define FIRE_BUFF_DURATION 2
#define WATER_BUFF_FROST 1
#define WATER_BUFF_REGENERATION_HEALTH 5
#define WATER_BUFF_REGENERATION_DURATION 2
#define EARTH_SHIELD 20
#define AIR_DURATION 1


class Player;

/* Structures */

class GameManager
{
    int socket_fd;
    //
    // Singletone
    //
        
    int connected;
    pthread_cond_t cond;
    pthread_mutex_t mut;

    public:

    std::vector<Player> players;

        GameManager();

        int getConnected() {    return connected;   }
        void setConnected(int connected);
        void gameStatus();
    
        int declineClients();
        
        void controlGame ();
        // Abbreviation for pthread library
        // functions
        void lock();
        void unlock();
        void wait();
        void broadcast();
            
        //
	    //  SPELLS
	    //

	    void lightSpell(int player_num);
	    void darkSpell(int player_num);
	    void fireSpell(int player_num);
	    void waterSpell(int player_num);
	    void earthSpell(int player_num);
	    void airSpell(int player_num);

        ~GameManager();
};

struct thread_args
{
    int num;
    int fd;
    GameManager *game;
};

/* Functions */
void game_status(int fd);
int scale_health (int health);

int saferead(int, void*, size_t);
int safewrite (int, const void*, size_t);
int readstr (int, char*, size_t);

#endif
