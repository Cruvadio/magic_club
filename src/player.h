#ifndef _PLAYER_H
#define _PLAYER_H

class Player
{
    Stats stat;
    int socket_fd
    public:
	    Player(): stat(), socket_fd(-1) {}
	    enum magic_t
	    {
		    NONE;
		    LIGHT;
		    DARK;
		    FIRE;
		    WATER;
		    EARTH;
		    AIR;
	    }
	    friend class Stats
	    {
		    int health;
		    int shield;
		    magic_t left_hand;
		    magic_t right_hand;

		    public:
		    Stats(): health(100), shield(0), left_hand(NONE), right_hand(NONE){}
		    int getHealth() {	return health;	}
		    int getShield() {	return shield;	}
		    void setHealth(int health);
		    void setShield(int shield);

		    magic_t getLeftHand();
		    magic_t getRightHand();
	    };

	    //
	    // SERVER METHODS
	    //

	    int getSocketFD() {	return socket_fd;   }
	    int acceptPlayer(int socket_fd);
	    void diconnectFromServer ();

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

enum stat_t
{
	NONE;
	HEALTH;
	SHIELD;
	DAMAGE;
}

class Buff
{
	int time;
	stat_t st;
	bool is_active;

};
#endif
