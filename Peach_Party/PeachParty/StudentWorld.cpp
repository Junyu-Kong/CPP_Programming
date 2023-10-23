#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Board.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

const int STEP = 2;
const int LEFT = 180;
const int RIGHT = 0;
const int UP = 90;
const int DOWN = 270;

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}
StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    // load board
    char board_num = getBoardNumber() + '0';
    string board_file = assetPath() + "board0" + board_num + ".txt";
    bd = new Board;
    Board::LoadResult result = bd->loadBoard(board_file);

    if (result != Board::load_success)
        return GWSTATUS_BOARD_ERROR;

    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 16; y++)
        {
            int pix_x = SPRITE_WIDTH * x;
            int pix_y = SPRITE_WIDTH * y;
            Board::GridEntry ge = bd->getContentsOf(x, y);
            switch (ge)
            {
            case Board::player:
                yoshi = new Yoshi(pix_x, pix_y, this);
                peach = new Peach(pix_x, pix_y, this);
                actors.push_back(new BlueCoin(pix_x, pix_y, this));
                break;
            case Board::blue_coin_square:
                actors.push_back(new BlueCoin(pix_x, pix_y, this));
                break;
            case Board::red_coin_square:
                actors.push_back(new RedCoin(pix_x, pix_y, this));
                break;
            case Board::star_square:
                actors.push_back(new Star(pix_x, pix_y, this));
                break;
            case Board::event_square:
                actors.push_back(new Event(pix_x, pix_y, this));
                break;
            case Board::bank_square:
                actors.push_back(new Bank(pix_x, pix_y, this));
                break;
            case Board::bowser:
                actors.push_back(new Bowser(pix_x, pix_y, this));
                actors.push_back(new BlueCoin(pix_x, pix_y, this));
                break;
            case Board::boo:
                actors.push_back(new Boo(pix_x, pix_y, this));
                actors.push_back(new BlueCoin(pix_x, pix_y, this));
                break;
            case Board::left_dir_square:
                actors.push_back(new Direction(pix_x, pix_y, this, LEFT));
                break;
            case Board::right_dir_square:
                actors.push_back(new Direction(pix_x, pix_y, this, RIGHT));
                break;
            case Board::up_dir_square:
                actors.push_back(new Direction(pix_x, pix_y, this, UP));
                break;
            case Board::down_dir_square:
                actors.push_back(new Direction(pix_x, pix_y, this, DOWN));
                break;
            }
        }

	startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit ESC.
    // Notice that the return value GWSTATUS_NOT_IMPLEMENTED will cause our framework to end the game.

    // PART 1: END OF GAME
    if (timeRemaining() <= 0)
    {
        playSound(SOUND_GAME_FINISHED);
        bool peach_won = true;
        if (yoshi != nullptr && peach != nullptr)
        {
            int ys = yoshi->getStar();
            int ps = peach->getStar();
            int yc = yoshi->getCoin();
            int pc = peach->getCoin();
            if (ys > ps)
            {
                peach_won = false;
                setFinalScore(ys, yc);
            }
            else if (ps > ys)
                setFinalScore(ps, pc);
            else
                if (yc > pc)
                {
                    peach_won = false;
                    setFinalScore(ys, yc);
                }
                else if (pc > yc)
                    setFinalScore(ps, pc);
                else
                    if (rand() % 2 == 0)
                    {
                        peach_won = false;
                        setFinalScore(ys, yc);
                    }
                    else
                        setFinalScore(ps, pc);
        }
        if (peach_won)
            return GWSTATUS_PEACH_WON;
        return GWSTATUS_YOSHI_WON;
    }

    // PART 2: doSomething
    if (yoshi != nullptr)
        yoshi->doSomething();
    if (peach != nullptr)
        peach->doSomething();
    vector<Actor*>::iterator p = actors.begin();
    while (!actors.empty() && p != actors.end())
    {
        if ((*p)->getLive())
            (*p)->doSomething();
        p++;
    }

    // PART 3: DELETE DEAD ACTORS
    p = actors.begin();
    while (!actors.empty())
    {
        if (!(*p)->getLive())
        {
            delete(*p);
            p = actors.erase(p);
        }
        else
            p++;
        if (p == actors.end())
            break;
    }

    // PART 4: UPDATE STATUS TEXT

    string t = "";
    if (peach != nullptr)
    {
        t += "P1 Roll : ";
        t += intToString(peach->getDie());
        t += " Stars: ";
        t += intToString(peach->getStar());
        t += " $$: ";
        t += intToString(peach->getCoin());
        t += peach->getVor() ? " VOR" :"";
        t += " | ";
    }
    t += "Time: ";
    t += intToString(timeRemaining());
    t += " | Bank: ";
    t += m_bank + '0';
    if (yoshi != nullptr)
    {
        t += " | P2 Roll: ";
        t += intToString(yoshi->getDie());
        t += " Stars: ";
        t += intToString(yoshi->getStar());
        t += " $$: ";
        t += intToString(yoshi->getCoin());
        t += yoshi->getVor() ? " VOR" : "";
    }
    setGameStatText(t);
    
    // PART 5: CONTINUE GAMING
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete peach;
    delete yoshi;
    vector<Actor*>::iterator p = actors.begin();
    while (!actors.empty())
    {
        delete (*p);
        p = actors.erase(p);
        if (p == actors.end())
            break;
    }
    delete bd;
}

void StudentWorld::addDrop(int x, int y)
{
    for (int i = 0; i < actors.size(); i++)
        if (actors.at(i)->getX() == x && actors.at(i)->getY() == y)
        {
            if (actors.at(i)->destoyable())
            {
                actors.at(i)->setLive(false);
            }
        }
    actors.push_back(new Dropping(x, y, this));

}
// help functions 
Board::GridEntry StudentWorld::getGridItem(int x, int y)
{
    return bd->getContentsOf(x, y);
}

string StudentWorld::intToString(int num) const
{
    string t = "";
    while (num > 0)
    {
        char c = num % 10 + '0';
        t = c + t;
        num /= 10;
    }

    return t == "" ? "0" : t;
}

bool StudentWorld::hitAny(int x, int y)
{
    int vortex_x_left = x;
    int vortex_x_right = x + 15;
    int vortex_y_down = y;
    int vortex_y_up = y + 15;
    for (int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->impactable())
        {
            int object_x_left = actors[i]->getX();
            int object_x_right = actors[i]->getX() + 15;
            int object_y_down = actors[i]->getY();
            int object_y_up = actors[i]->getY() + 15;
            if (((vortex_x_left >= object_x_left && vortex_x_left <= object_x_right) || (vortex_x_right >= object_x_left && vortex_x_right <= object_x_right))
                && ((vortex_y_down >= object_y_down && vortex_y_down <= object_y_up) || (vortex_y_up >= object_y_down && vortex_y_up <= object_y_up)))
            {
                actors[i]->setImpact(true);
                return true;
            }
        }
    }
    return false;
}

void StudentWorld::createVortex(int x, int y, int dir)
{
    actors.push_back(new Vortex(x, y, this, dir));
}