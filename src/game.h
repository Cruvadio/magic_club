#ifndef _GAME_H
#define _GAME_H
#include <pthread.h>

#define SERVER_NAME "Welcome to 'Mage Club' ver 0.0.3!\n\r"
#define BUSY_MSG "'Mage Club Server ver 0.0.3: Sorry, the game has already started\n\r"
#define PROMT ">> "
#define SORRY "Sorry, another player has disconnected. Game over\n\r"
#define WAIT "Waiting for oppponent...\n\r"
#define FOUND "Another player has found\n\r"
#define MAX_CONNECTIONS 2
#define MAX_HEALTH 100
#define MIN_HEALTH 0

/* Statuses */
#define READY 1
#define WAITING 2

/* Structures */
typedef struct
{
    int health;
    int shield;
}stat;

typedef struct
{
    int num;
    int status;
    stat st;
    int fd;
}thread_args;

extern pthread_mutex_t mut;
extern pthread_cond_t cond;
extern thread_args args[MAX_CONNECTIONS];

/* Functions */
void game_status(int fd);
int scale_health (int health);

int saferead(int, void*, size_t);
int safewrite (int, const void*, size_t);
int readstr (int, char*, size_t);

#endif
