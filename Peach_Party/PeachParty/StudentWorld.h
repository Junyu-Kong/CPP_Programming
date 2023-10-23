#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Peach;
class Yoshi;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// helper
	bool hitAny(int x, int y);
	void createVortex(int x, int y, int dir);

	// getters
	 Board::GridEntry getGridItem(int x, int y);
	 Peach* getPeach() const { return peach; }
	 Yoshi* getYoshi() const { return yoshi; }
	 Board* getBoard() const { return bd; }
	 int getBank() const { return m_bank; }
	 std::string intToString(int num) const;

	// setters
	void clearBank() { m_bank = 0; }
	void addBank(int num) { m_bank += num; }
	void addDrop(int x, int y);

private:
	// data members
	Board* bd = nullptr;
	std::vector <Actor*> actors;
	Peach* peach = nullptr;
	Yoshi* yoshi = nullptr;
	int m_bank = 0;
};
#endif // STUDENTWORLD_H_
