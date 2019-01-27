#ifndef _PLAYER_H
#define _PLAYER_H
#include "game.h"
#include <vector>
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

enum mode_t
{
    WAITING,
    READY
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
    mode_t mode;
    bool is_alive;
   
    void scaleHealth ();
    public:
	    Player(): stat(), socket_fd(-1), buffs(),mode(WAITING), is_alive(true) {}
	   
	    //
	    // SERVER METHODS
	    //

	    int getSocketFD() {	return socket_fd;   }
	    int acceptPlayer(int socket_fd);
	    void diconnectFromServer ();
        mode_t getMode() { return mode; }
        const char* handToString(int hand);
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
