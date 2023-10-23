#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

const int STEP = 2;
const int LEFT = 180;
const int RIGHT = 0;
const int UP = 90;
const int DOWN = 270;

// Actor functions 
int Actor::updateDir(int dir, int x, int y)
{	 

	switch (dir)
	{
	case RIGHT:
	case LEFT:
		if (movable(dir))
			return dir;
		if (movable(UP))
			return UP;
		return DOWN;
	case UP:
	case DOWN:
		if (movable(dir))
			return dir;
		if (movable(RIGHT))
			return RIGHT;
		return LEFT;
	default:
		return dir;
	}
}

bool Actor::movable(int direction) const
{
	int x = getX();
	int y = getY();
	switch (direction)
	{
	case RIGHT:
		if (x <= VIEW_WIDTH - SPRITE_WIDTH - STEP && this->getWorld()->getGridItem((x + SPRITE_WIDTH - 1 + STEP) / 16, y / 16) != Board::empty)
			return true;
		return false;
		break;
	case LEFT:
		if (x < STEP || this->getWorld()->getGridItem((x - STEP) / 16, y / 16) == Board::empty)
			return false;
		return true;
		break;
	case UP:
		if (y <= VIEW_HEIGHT - SPRITE_HEIGHT - STEP && this->getWorld()->getGridItem(x / 16, (y + SPRITE_HEIGHT - 1 + STEP) / 16) != Board::empty)
			return true;
		return false;
		break;
	case DOWN:
		if (y < STEP || this->getWorld()->getGridItem(x / 16, (y - STEP) / 16) == Board::empty)
			return false;
		return true;
		break;
	default:
		return false;
	}
}

void Actor::updateSprite() {
		if (getDir() == LEFT)
			setDirection(LEFT);
		else
			setDirection(RIGHT);
}

bool Actor::landOn(Avatar* actor) const
{
	if (actor != nullptr && actor->getX() == getX() && actor->getY() == getY() && actor->getWait())
		return true;
	return false;
}

bool Actor::moveOn(Avatar* actor) const
{
	if (actor != nullptr && actor->getX() == getX() && actor->getY() == getY() && !actor->getWait())
		return true;
	return false;
}

bool Actor::cross(Actor* actor) const
{
	if (actor != nullptr && actor->getX() == getX() && actor->getY() == getY())
		return true;
	return false;
}

void Actor::swap(Avatar* a1, Avatar* a2)
{
	int x = a1->getX();
	int y = a1->getY();
	a1->moveTo(a2->getX(), a2->getY());
	a2->moveTo(x, y);

	int ticks = a1->getTicks();
	a1->setTicks(a2->getTicks());
	a2->setTicks(ticks);

	int dir = a1->getDir();
	a1->setDir(a2->getDir());
	a2->setDir(dir);

	int direction = a1->getDirection();
	a1->setDirection(a2->getDirection());
	a2->setDirection(direction);

	bool wait = a1->getWait();
	a1->setWait(a2->getWait());
	a2->setWait(wait);

	bool newP = a1->getNew();
	a1->setNew(a2->getNew());
	a2->setNew(newP);

	Coord c = a1->getCoor();
	a1->setCoord(a2->getCoor());
	a2->setCoord(c);

	bool start = a1->getStart();
	a1->setStart(a2->getStart());
	a2->setStart(start);
}
// Avatar functions 
void Avatar::swapCoin()
{
	StudentWorld* sw = getWorld();
	Peach* p = sw->getPeach();
	Yoshi* y = sw->getYoshi();

	int temp = p->getCoin();
	p->setCoin(y->getCoin());
	y->setCoin(temp);
}

void Avatar::swapStar()
{
	StudentWorld* sw = getWorld();
	Peach* p = sw->getPeach();
	Yoshi* y = sw->getYoshi();

	int temp = p->getStar();
	p->setStar(y->getStar());
	y->setStar(temp);
}

int Avatar::deCode(int num)
{
	switch (num)
	{
	case ACTION_DOWN:
		return DOWN;
		break;
	case ACTION_LEFT:
		return LEFT;
		break;
	case ACTION_RIGHT:
		return RIGHT;
		break;
	case ACTION_UP:
		return UP;
		break;
	default:
		return -1;
	}
}

int Avatar::previousDir()
{
	int cur_x = getX();
	int cur_y = getY();
	int prev_x = last_step.x();
	int prev_y = last_step.y();
	if (cur_x > prev_x)
		return RIGHT;
	if (cur_x < prev_x)
		return LEFT;
	if (cur_y > prev_y)
		return UP;
	if (cur_y < prev_y)
		return DOWN;
	return -1;
}

void Avatar::doSomething(int num)
{
	// part 1
	if (m_wait_to_roll)
	{

		// 1.a
		setDir(updateDir(getDir(), getX(), getY()));
		updateSprite();

		// 1.b
		int action = getWorld()->getAction(num);
		
		// 1.c
		if (action == ACTION_ROLL)
		{
			int die_roll = rand() % 10 + 1;
			m_ticks_to_move = die_roll * 8;
			m_wait_to_roll = false;
		}

		// 1.d
		else if (action == ACTION_FIRE && m_vortex)
		{
			getWorld()->createVortex(getX(), getY(), getDir());
			getWorld()->playSound(SOUND_PLAYER_FIRE);
			m_vortex = false;
		}

		// 1.e
		else
			return;
		
	}

	// part 2
	if (!m_wait_to_roll)
	{	
		m_newPlayer = true;
		int x = getX();
		int y = getY();
		std::vector<int> fork;
		for (int i = 0; i < 360; i += 90)
			if (movable(i))
				fork.push_back(i);
		std::vector<int>::iterator it = std::find(fork.begin(), fork.end(), oppositeDir(previousDir()));
		if (it != fork.end())
			fork.erase(it);

		// no need to do anything if not directly on top of a suqare
		if (!(x % 16 == 0 && y % 16 == 0))
			;

		// 2.a
		else if (getWorld()->getBoard()->getContentsOf(x/16, y/16) == Board::left_dir_square
			|| getWorld()->getBoard()->getContentsOf(x/16, y/16) == Board::right_dir_square
			|| getWorld()->getBoard()->getContentsOf(x/16, y/16) == Board::up_dir_square
			|| getWorld()->getBoard()->getContentsOf(x/16, y/16) == Board::down_dir_square)
		{

		}

		// 2.b
		else if (fork.size() > 1 && !m_justStart)
		{
			int action = deCode(getWorld()->getAction(num));
			if (std::find(fork.begin(), fork.end(), action) != fork.end())
			{
				setDir(action);
				updateSprite();
			}
			else
				return;
		}

		// check if gets teleported
		else if (getTele())
		{
			std::vector<int> dir;
			for (int i = 0; i < 360; i += 90)
				if (movable(i))
					dir.push_back(i);
			int action = deCode(getWorld()->getAction(num));
			if (std::find(dir.begin(), dir.end(), action) != dir.end())
			{
				setDir(action);
				updateSprite();
				setTele(false);
			}
			else
				return;
		}
		// 2.c
		else if (updateDir(getDir(), getX(), getY()) != getDir())
		{
			setDir(updateDir(getDir(), getX(), getY()));
			updateSprite();
		}


		// 2.d
		last_step = Coord(x, y);
		moveAtAngle(getDir(), 2);
		m_justStart = false;

		// 2.e
		m_ticks_to_move--;

		// 2.f
		if (m_ticks_to_move == 0)
			m_wait_to_roll = true;
	}
}

// Coin functions 
void BlueCoin::sound() const
{
	getWorld()->playSound(SOUND_GIVE_COIN);
}

void RedCoin::sound() const
{
	getWorld()->playSound(SOUND_TAKE_COIN);
}

void Coin::doSomething(int coin, int star)
{
	// part 1
	if (!getLive())
		return;

	// part 2
	Peach* p = getWorld()->getPeach();
	Yoshi* y = getWorld()->getYoshi();
	if (star == 1)
	{
		if (landOn(p) && p->getNew())
		{
			p->addCoin(coin);
			if (p->getCoin() < 0)
				p->addCoin(p->getCoin() * -1);
			sound();
			p->setNew(false);
		}
		if (landOn(y) && y->getNew())
		{
			y->addCoin(coin);
			if (p->getCoin() < 0)
				p->addCoin(p->getCoin() * -1);
			sound();
			y->setNew(false);
		}
	}
	else
	{
		if (landOn(p) && p->getNew())
		{
			p->addStar(star);
			if (p->getStar() < 0)
				p->addStar(p->getStar() * -1);
			sound();
			p->setNew(false);
		}
		if (landOn(y) && y->getNew())
		{
			y->addStar(star);
			if (p->getStar() < 0)
				p->addStar(p->getStar() * -1);
			sound();
			y->setNew(false);
		}
	}


}

// Dropping functions
void Dropping::doSomething()
{
	int option = rand() % 2 + 1;
	if (option == 1)
		Coin::doSomething(-10);
	else
		Coin::doSomething(0, -1);
}

void Dropping::sound() const
{
	getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

// Star functions 
void Star::doSomething()
{
	Peach* p = getWorld()->getPeach();
	Yoshi* y = getWorld()->getYoshi();

	if (cross(p) && p->getNew())
	{
		if (p->getCoin() >= 20)
		{
			p->addCoin(-20);
			p->addStar(1);
			getWorld()->playSound(SOUND_GIVE_STAR);
		}
		p->setNew(false);
	}
	if (cross(y) && y->getNew())
	{
		if (y->getCoin() >= 20)
		{
			y->addCoin(-20);
			y->addStar(1);
			getWorld()->playSound(SOUND_GIVE_STAR);
		}
		y->setNew(false);
	}
}

// Direction functions 
void Direction::doSomething()
{
	Peach* p = getWorld()->getPeach();
	Yoshi* y = getWorld()->getYoshi();

	if (cross(p))
	{
		p->setDir(getDirection());
		p->updateSprite();
	}
	if (cross(y))
	{
		y->setDir(getDirection());
		y->updateSprite();
	}
}

// Bank functions 
void Bank::doSomething()
{
	StudentWorld* sw = getWorld();
	std::vector <Avatar*> avatars;
	avatars.push_back(sw->getPeach());
	avatars.push_back(sw->getYoshi());

	for (std::vector<Avatar*>::iterator a = avatars.begin(); a != avatars.end(); a++)
	{
		if (landOn(*a) && (*a)->getNew())
		{
			(*a)->addCoin(sw->getBank());
			sw->clearBank();
			(*a)->setNew(false);
			sw->playSound(SOUND_WITHDRAW_BANK);
		}
		else if (moveOn(*a))
		{
			int coin = (*a)->getCoin();
			if (coin < 5)
			{
				sw->addBank(coin);
				(*a)->addCoin(coin * -1);
			}
			else
			{
				sw->addBank(5);
				(*a)->addCoin(-5);
			}
			sw->playSound(SOUND_DEPOSIT_BANK);
		}
	}
}

// Event functions
void Event::doSomething()
{
	StudentWorld* sw = getWorld();
	std::vector<Avatar*> avatars;
	avatars.push_back(sw->getPeach());
	avatars.push_back(sw->getYoshi());

	for (std::vector<Avatar*>::iterator avatar = avatars.begin(); avatar != avatars.end(); avatar++)
	{
		if (landOn(*avatar) && (*avatar)->getNew())
		{
			int choice = rand() % 3 + 1;
			switch (choice)
			{
			case 1:
			{
				// find a valid teleport position
				int x = rand() % 16;
				int y = rand() % 16;
				Board* bd = sw->getBoard();
				while (bd->getContentsOf(x, y) == Board::empty)
				{
					x = rand() % 16;
					y = rand() % 16;
				}

				// teleport it
				(*avatar)->moveTo(x*16, y*16);
				(*avatar)->setTele(true);
				sw->playSound(SOUND_PLAYER_TELEPORT);
				break;
			}
			case 2:
				(*avatar)->setNew(false);
				swap(avatars[0], avatars[1]);
				sw->playSound(SOUND_PLAYER_TELEPORT);
				break;
			case 3:
				(*avatar)->setVor(true);
				(*avatar)->setNew(false);
				sw->playSound(SOUND_GIVE_VORTEX);
				break;
			}
		}
	}
	
}

// Baddie functions
void Baddie::teleport()
{
	// find a valid teleport position
	int x = rand() % 16;
	int y = rand() % 16;
	Board* bd = getWorld()->getBoard();
	while (bd->getContentsOf(x, y) == Board::empty)
	{
		x = rand() % 16;
		y = rand() % 16;
	}

	moveTo(x * 16, y * 16);
	setWalk(false);
	setDir(RIGHT);
	updateSprite();
	setPause(180);
}

void Baddie::doSomething()
{
	if (getImpact())
	{
		teleport();
		setImpact(false);
	}
	Peach* p = getWorld()->getPeach();
	Yoshi* y = getWorld()->getYoshi();
	// part 1
	if (!getWalk())
	{
		// 1.a
		if (landOn(p) && getLastPeach().x() != getX() && getLastPeach().y() != getY())
		{
			setLastPeach(Coord(getX(), getY()));
			int random = rand() % 2;
			inTouch(random, p);
		}
		if (landOn(y) && getLastYoshi().x() != getX() && getLastYoshi().y() != getY())
		{
			setLastYoshi(Coord(getX(), getY()));
			int random = rand() % 2;
			inTouch(random, p);
		}

		// 1.b
		decrementPause();

		// 1.c
		if (getPause() == 0)
		{
			int squares_to_move = squareToMove();
			setTick(squares_to_move * 8);
			std::vector<int> directions;
			for (int i = 0; i < 360; i += 90)
				if (movable(i))
					directions.push_back(i);
			int random = rand() % directions.size();
			setDir(directions.at(random));
			updateSprite();
			setWalk(true);
		}
	}

	// part 2
	if (getWalk())
	{
		// 2.a
		int x = getX();
		int y = getY();
		std::vector<int> fork;
		for (int i = 0; i < 360; i += 90)
			if (movable(i))
				fork.push_back(i);
		// no need to do anything if not directly on top of a suqare
		if (!(x % 16 == 0 && y % 16 == 0))
			;
		else if (fork.size() > 2)
		{
			setDir(fork.at(rand() % fork.size()));
			updateSprite();
		}

		// 2.b
		else if (!movable(getDir()))
		{
			switch (getDir())
			{
			case UP:
			case DOWN:
				if (movable(RIGHT))
					setDir(RIGHT);
				else
					setDir(LEFT);
				break;
			case LEFT:
			case RIGHT:
				if (movable(UP))
					setDir(UP);
				else
					setDir(DOWN);
				break;
			}
			updateSprite();
		}

		// 2.c
		moveAtAngle(getDir(), 2);

		// 2.d
		decrementTick();

		// 2.d
		if (getTick() == 0)
		{
			std::cerr << std::endl;
			setWalk(false);
			setPause(180);
			zeroTick();
		}
	}
}
// Bowser functions
void Bowser::inTouch(int num, Avatar* av)
{
	if (num == 0)
	{
		av->clearCoin();
		av->clearStar();
		getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
	}
}

void Bowser::zeroTick()
{
	int random = rand() % 4;
	if (random == 0)
	{
		getWorld()->addDrop(getX(), getY());
		getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
	}
}
// Boo functions
void Boo::inTouch(int num, Avatar* av)
{
	if (num == 0)
		av->swapCoin();
	else
		av->swapStar();
	getWorld()->playSound(SOUND_BOO_ACTIVATE);
}

// Vortex functions
void Vortex::doSomething()
{
	// part 1
	if (!getLive())
		return;

	// part 2
	moveAtAngle(getDir(), 2);

	// part 3
	if (getX() >= VIEW_HEIGHT || getX() < 0 || getY() >= VIEW_WIDTH || getY() < 0)
		setLive(false);

	// part 4
	bool hit = getWorld()->hitAny(getX(), getY());
	if (hit)
	{
		setLive(false);
		getWorld()->playSound(SOUND_HIT_BY_VORTEX);
	}


}