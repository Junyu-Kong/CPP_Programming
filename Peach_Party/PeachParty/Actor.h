#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <queue>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;
class Avatar;

class Coord
{
public:
	Coord(int x, int y) : m_x(x), m_y(y) {}
	Coord(const Coord& c) : m_x(c.x()), m_y(c.y()) {}
	int x() const { return m_x; }
	int y() const { return m_y; }
private:
	int m_x;
	int m_y;
};

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* world, int dir = right, int depth = 0, double size = 1.0)
		:GraphObject(imageID, startX, startY, dir, depth, size), m_world(world) {}
	virtual ~Actor(){}
	virtual void doSomething() = 0;

	// help functions
	bool moveOn(Avatar* actor) const;
	bool landOn(Avatar* actor) const;
	bool cross(Actor* actor) const;
	bool movable(int direction) const;
	void swap(Avatar* a1, Avatar* a2);

	// getters
	int getDir() const { return m_walk_dir; }  // walking direction
	bool getLive() const { return m_alive; }
	StudentWorld* getWorld() const { return m_world; }
	virtual bool impactable() const { return false; }
	virtual bool destoyable() const { return true; }
	bool getImpact() const { return m_impacted; }

	// setters
	void setDir(int dir) { m_walk_dir = dir; }  // walking direction
	void setLive(bool live) { m_alive = live; }
	int updateDir(int dir, int x, int y);  // walking direction
	void updateSprite();
	void setImpact(bool state) { m_impacted = state; }

private:
	StudentWorld* m_world;
	int m_walk_dir = right;
	bool m_alive = true;
	bool m_impacted = false;
};

// Avatar
class Avatar : public Actor
{
public:
	Avatar(int imageID, int startX, int startY, StudentWorld* world)
		:Actor(imageID, startX, startY, world), last_step(startX, startY) {}
	virtual ~Avatar() {}
	virtual void doSomething() = 0;


	// help functions
	void doSomething(int playerNum);
	void swapCoin();
	void swapStar();


	// getter
	int getStar() const { return m_star; }
	int getCoin() const { return m_coin; }
	bool getWait() const { return m_wait_to_roll; }
	bool getNew() const { return m_newPlayer; }
	int getDie() const { return (m_ticks_to_move + 6) / 8; }
	int getVor() const { return m_vortex; }
	int getTicks() const { return m_ticks_to_move; }
	Coord getCoor() const { return last_step; }
	bool getStart() const { return m_justStart; }
	virtual bool destoyable() const { return false; }
	bool getTele() const { return m_tele; }

	// setter
	void clearStar() { m_star = 0; }
	void clearCoin() { m_coin = 0; }
	void addStar(int num) { m_star += num; }
	void setStar(int num) { m_star = num; }
	void addCoin(int num) { m_coin += num; }
	void setCoin(int num) { m_coin = num; }
	void setNew(bool newP) { m_newPlayer = newP; }
	void setTicks(int ticks) { m_ticks_to_move = ticks;}
	void setWait(bool state) { m_wait_to_roll = state; }
	void setCoord(Coord c) { last_step = c; }
	void setStart(bool start) { m_justStart = start; }
	void setVor(bool state) { m_vortex = state; }
	void setTele(bool state) { m_tele = state; }

private:
	int m_coin = 0;
	int m_star = 0;
	bool m_vortex = false;
	int m_ticks_to_move = 0;
	bool m_wait_to_roll = true;
	bool m_newPlayer = true;
	bool m_justStart = true;
	bool m_tele = false;
	Coord last_step;

	int deCode(int num);
	int oppositeDir(int num) { return (num + 180) % 360; }
	int previousDir();
};

class Peach : public Avatar
{
public:
	Peach(int startX, int startY, StudentWorld* world)
		:Avatar(IID_PEACH, startX, startY, world) {}
	virtual ~Peach() {}
	virtual void doSomething() { Avatar::doSomething(1); }
};

class Yoshi : public Avatar
{
public:
	Yoshi(int startX, int startY, StudentWorld* world)
		:Avatar(IID_YOSHI, startX, startY, world) {}
	virtual ~Yoshi() {}
	virtual void doSomething() { Avatar::doSomething(2); }
};

// Coin 
class Coin : public Actor
{
public:
	Coin(int imageID, int startX, int startY, StudentWorld* world, int dir = right, int depth = 1)
		:Actor(imageID, startX, startY, world, dir, depth) {}
	virtual ~Coin() {}
	void doSomething(int coin, int star = 1);
	virtual void sound() const = 0;
	virtual void doSomething() = 0;

private:

};

class BlueCoin : public Coin
{
public:
	BlueCoin(int startX, int startY, StudentWorld* world)
		:Coin(IID_BLUE_COIN_SQUARE, startX, startY, world) {}
	virtual ~BlueCoin() {}
	virtual void doSomething() { Coin::doSomething(3); }
	virtual void sound() const;
};

class RedCoin : public Coin
{
public:
	RedCoin(int startX, int startY, StudentWorld* world)
		:Coin(IID_RED_COIN_SQUARE, startX, startY, world) {}
	virtual ~RedCoin() {}
	virtual void doSomething() { Coin::doSomething(-3); }
	virtual void sound() const;
};

// Dropping
class Dropping : public Coin
{
public:
	Dropping(int startX, int startY, StudentWorld* world)
		:Coin(IID_DROPPING_SQUARE, startX, startY, world) {}
	virtual ~Dropping() {}
	virtual void doSomething();
	virtual void sound() const;
};

// Star 
class Star : public Actor
{
public:
	Star(int startX, int startY, StudentWorld* world, int dir = right, int depth = 1)
		:Actor(IID_STAR_SQUARE, startX, startY, world, dir, depth) {}
	virtual ~Star() {}
	virtual void doSomething();
};

// Direction 
class Direction : public Actor
{
public:
	Direction(int startX, int startY, StudentWorld* world, int dir = right, int depth = 1)
		:Actor(IID_DIR_SQUARE, startX, startY, world, dir, depth) {}
	virtual ~Direction() {}
	virtual void doSomething();
};

// Bank 
class Bank : public Actor
{
public:
	Bank(int startX, int startY, StudentWorld* world, int dir = right, int depth = 1)
		:Actor(IID_BANK_SQUARE, startX, startY, world, dir, depth) {}
	virtual ~Bank() {}
	virtual void doSomething();
};

// Event
class Event : public Actor
{
public:
	Event(int startX, int startY, StudentWorld* world, int dir = right, int depth = 1)
		:Actor(IID_EVENT_SQUARE, startX, startY, world, dir, depth) {}
	virtual ~Event() {}
	virtual void doSomething();
};

class Baddie : public Actor
{
public:
	Baddie(int imageID, int startX, int startY, StudentWorld* world)
		:Actor(imageID, startX, startY, world) {}
	virtual ~Baddie() {}
	virtual void doSomething();

	// helpers
	virtual void inTouch(int num, Avatar* av) = 0;
	virtual int squareToMove() = 0;
	void teleport();
	virtual void zeroTick() { ; }

	// getters
	bool getWalk() const { return m_walk; }
	int getPause() const { return m_pause; }
	int getTravel() const { return m_travel; }
	virtual bool impactable() const { return true; }
	int getTick() const { return m_ticks_to_move; }
	Coord getLastPeach() const { return m_last_active_peach; }
	Coord getLastYoshi() const { return m_last_active_yoshi; }
	virtual bool destoyable() const { return false; }

	// setters
	void decrementPause() { m_pause--; }
	void decrementTick() { m_ticks_to_move--; }
	void setPause(int num) { m_pause = num; }
	void setTick(int tick) { m_ticks_to_move = tick; }
	void setLastYoshi(Coord c) { m_last_active_yoshi = c; }
	void setLastPeach(Coord c) { m_last_active_peach = c; }
	void setWalk(bool state) { m_walk = state; }

private:
	bool m_walk = false;
	int m_pause = 180;
	int m_travel = 0;
	int m_ticks_to_move = 0;
	Coord m_last_active_peach = Coord(-1, -1);
	Coord m_last_active_yoshi = Coord(-1, -1);
};

class Bowser : public Baddie
{
public:
	Bowser(int startX, int startY, StudentWorld* world)
		:Baddie(IID_BOWSER, startX, startY, world) {}
	virtual ~Bowser() {}
	virtual void doSomething() { Baddie::doSomething(); }
	virtual void inTouch(int num, Avatar* av);
	virtual int squareToMove() { return rand() % 10 + 1; }
	virtual void zeroTick();
};

class Boo : public Baddie
{
public:
	Boo(int startX, int startY, StudentWorld* world)
		:Baddie(IID_BOO, startX, startY, world) {}
	virtual ~Boo() {}
	virtual void doSomething() { Baddie::doSomething(); }
	virtual void inTouch(int num, Avatar* av);
	virtual int squareToMove() { return rand() % 3 + 1; }
};

// vortex
class Vortex : public Actor
{
public:
	Vortex(int startX, int startY, StudentWorld* world, int dir)
		:Actor(IID_VORTEX, startX, startY, world) {
		setDir(dir);
	}
	virtual ~Vortex() {}
	virtual void doSomething();

};
#endif // ACTOR_H_
