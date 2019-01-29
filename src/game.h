#pragma once

#include "player.h"
#ifndef _GAME_H

#define _GAME_H

#include <pthread.h>


#define MAX_CONNECTIONS 2
#define MAX_HEALTH 100
#define MIN_HEALTH 0


/* Structures */
enum number_t
{
    FIRST = 1,
    SECOND
};

class GameManager
{
    int socket_fd;
    //
    // Singletone
    //
    static GameManager & game;
    
    int connected;
    pthread_cond_t cond;
    pthread_mutex_t mut;

    public:
    
    Player players[MAX_CONNECTIONS];

        GameManager();

        int getConnected() {    return connected;   }
        void setConnected(int connected);
        void gameStatus();
    
        void* acceptClients(void*); // For clients-threads
        int declineClients();
        
        void* waitForPlayers(void*); // For server-thread
        
        static GameManager& getGameManager(){  return game;    }
        // Abbreviation for pthread library
        // functions
        void lock();
        void unlock();
        void wait();
        void broadcast();

        ~GameManager();
};


/* Functions */
void game_status(int fd);
int scale_health (int health);

int saferead(int, void*, size_t);
int safewrite (int, const void*, size_t);
int readstr (int, char*, size_t);

#endif
