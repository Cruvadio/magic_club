#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include <pthread.h>


void GameManager::gameStatus()
{
    int fd;
    int i;
    char str[1024];
    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        fd = players[i].getSocketFD();
        if (!i)
            sprintf(str, "First player status:\n\r");
        else
            sprintf(str, "Second player status:\n\r");
        write(fd, str, strlen(str));
        sprintf(str, "Health: %d\n\r", players[i].getHealth());
        write(fd, str, strlen(str));
        sprintf(str, "Shield: %d\n\r", players[i].getShield());
        write(fd, str, strlen(str));
        if (players[i].getLeftHand() != m_NONE)
    }
}
