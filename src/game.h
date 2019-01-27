#ifndef _GAME_H

#define _GAME_H

#include <pthread.h>
#include "player.h"


#define SERVER_NAME "Welcome to 'Mage Club' ver 0.0.3!\n\r"
#define BUSY_MSG "'Mage Club Server ver 0.0.3: Sorry, the game has already started\n\r"
#define PROMT ">> "
#define SORRY "Sorry, another player has disconnected. Game over\n\r"
#define WAIT "Waiting for oppponent...\n\r"
#define FOUND "Another player has found\n\r"
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
    static GameManager game;

    pthread_cond_t cond;
    pthread_mutex_t mut;

    Player players[MAX_CONNECTIONS];

    public:
        GameManager();
        
        void gameStatus();

        int acceptClients();
        int declineClients();

        void lock();
        void unlock();
        void wait();
        void broadcast();
};


/* Functions */
void game_status(int fd);
int scale_health (int health);

int saferead(int, void*, size_t);
int safewrite (int, const void*, size_t);
int readstr (int, char*, size_t);

#endif
