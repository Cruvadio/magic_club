#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include <pthread.h>


void game_status(int fd)
{
    int i;
    char str[1024];
    pthread_mutex_lock(&mut);
    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (!i)
            sprintf(str, "First player status:\n\r");
        else
            sprintf(str, "Second player status:\n\r");
        write(fd, str, strlen(str));
        sprintf(str, "Health: %d\n\r", args[i].st.health);
        write(fd, str, strlen(str));
        sprintf(str, "Shield: %d\n\r", args[i].st.shield);
        write(fd, str, strlen(str));
    }
    pthread_mutex_unlock(&mut);
}

int scale_health (int health)
{
    if (health >= MAX_HEALTH)
        return MAX_HEALTH;
    else if (health <= MIN_HEALTH)
        return MIN_HEALTH;
    else
        return health;
}
