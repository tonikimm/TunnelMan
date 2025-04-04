#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;


//=================================== ACTOR BASE CLASS ===================================
Actor::Actor(int imageID, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* studentworld)
    :GraphObject(imageID, x, y, dir, size, depth), isalive(true), world(studentworld)
{
    setVisible(true);
}

Actor::~Actor()
{
    setVisible(false);
}

void Actor::moveTo(int x, int y)
{
    x=(x<0)?0:x;
    x=(x > 60)?60:x;
    y=(y<0)?0:y;
    y=(y>60)?60:y;
    GraphObject::moveTo(x, y);
}


//=================================== HUMAN CLASS ===================================
Human::Human(StudentWorld* world, int imageID, int x, int y, Direction dir, int hitpoint)
:Actor(imageID, x, y, dir, 1.0, 0, world), health(hitpoint){};


void Human::moveInDirection(Direction direction)
{
    switch (direction)
    {
        case left:
            if (getDirection() == left) {
                if (!getWorld()->isBoulder(getX() - 1, getY()))
                    moveTo(getX() - 1, getY());
                else
                    moveTo(getX(), getY());
            }
            else
                setDirection(left);
            break;
        case right:
            if (getDirection() == right) {
                if (!getWorld()->isBoulder(getX() + 1, getY()))
                    moveTo(getX() + 1, getY());
                else
                    moveTo(getX(), getY());
            }
            else
                setDirection(right);
            break;
        case up:
            if (getDirection() == up) {
                if (!getWorld()->isBoulder(getX(), getY() + 1))
                    moveTo(getX(), getY() + 1);
                else
                    moveTo(getX(), getY());
            }
            else
                setDirection(up);
            break;
        case down:
            if (getDirection() == down) {
                if (!getWorld()->isBoulder(getX(), getY() - 1))
                    moveTo(getX(), getY() - 1);
                else
                    moveTo(getX(), getY());
            }
            else
                setDirection(down);
            break;
        case none:
            return;
    }
}


//=================================== TUNNELMAN ===================================
TunnelMan::TunnelMan(StudentWorld* world)
    :Human(world, TID_PLAYER, 30, 60, right, 10)
{
    squirts=5;
    sonar=1;
    gold=0;
}


void TunnelMan::doSomething()
{
    if (!isAlive())
        return;
    if (getWorld()->removeDirt(getX(), getY()))
        getWorld()->playSound(SOUND_DIG);
    int input;
    if (getWorld()->getKey(input) == true)
    {
        switch (input)
        {
        case KEY_PRESS_UP:
            moveInDirection(up);
            break;
        case KEY_PRESS_DOWN:
            moveInDirection(down);
            break;
        case KEY_PRESS_LEFT:
            moveInDirection(left);
             break;
        case KEY_PRESS_RIGHT:
            moveInDirection(right);
            break;
        case KEY_PRESS_SPACE:
            if (squirts > 0)
            {
                squirts--;
                shoot();
            }
            break;
        case 'z':
        case 'Z': // not case sensitive
            if (sonar > 0)
            {
                sonar--;
                getWorld()->detectNearActors(getX(), getY(), 12);
                getWorld()->playSound(SOUND_SONAR);
            }
            break;
        case KEY_PRESS_ESCAPE:
            setDead();
            break;
        case KEY_PRESS_TAB:
            if (gold > 0)
            {
                getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
                gold--;
            }
            break;
        }
    }
}

void TunnelMan::inc(int id)
{
    if (id == TID_GOLD)
        gold++;
    if (id == TID_WATER_POOL)
        squirts+=5;
    if (id == TID_SONAR)
        sonar += 2;
}

void TunnelMan::shoot()
{
    switch (getDirection())
    {
        case left:
            if(!getWorld()->isEarth(getX()-4,getY())&& !getWorld()->isBoulder(getX()-4, getY()))
                getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
            break;
        case right:
            if (!getWorld()->isEarth(getX() + 4, getY()) && !getWorld()->isBoulder(getX()+4, getY()))
                getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
            break;
        case down:
            if (!getWorld()->isEarth(getX() , getY()-4) && !getWorld()->isBoulder(getX(), getY()-4))
                getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
            break;
        case up:
            if (!getWorld()->isEarth(getX(), getY() + 4) && !getWorld()->isBoulder(getX(), getY()+4 ))
                getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
            break;
        case none:
            return;
    }
    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}

void TunnelMan::isAnnoyed(int hitpoint)
{
    setHealth(hitpoint);
    if (getHealth()<=0)
    {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}


//=================================== EARTH ===================================
Earth::Earth(int x, int y, StudentWorld* world)
:Actor(TID_EARTH, x, y, right, 0.25, 3, world){};


//=================================== BOULDER ===================================
Boulder::Boulder(int x, int y, StudentWorld* world)
    : Actor(TID_BOULDER, x, y, down, 1.0, 1, world),stable(true),ticks(0)
{
    world->removeDirt(x, y);
}

void Boulder::doSomething()
{
    if (!isAlive())
        return;
    if (stable) {
        if (getWorld()->isSupported(getX(), getY() - 1))
            return;
        else stable = false;
    }
    if (ticks == 30) {
        isFalling = true;
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
    ticks++;
    if (isFalling) {
        if (getWorld()->isSupported(getX(), getY()-1)|| getWorld()->isBoulder(getX(), getY()-4, 0) || getY()==0)
            setDead();
        else moveTo(getX(), getY() - 1);
        if (!haveHit)
            hitMan();
    }
}

void Boulder::hitMan()
{
    if (getWorld()->playerTooClose(this, 3))
        getWorld()->getPlayer()->isAnnoyed(100);
    Protester * p = getWorld()->protesterTooClose(this, 3);
    if (p != nullptr) {
        haveHit=true;
        p->isAnnoyed(100);
    }
}

//=================================== SQUIRT ===================================
Squirt::Squirt(StudentWorld* world, int x, int y, Direction dir)
:Actor(TID_WATER_SPURT, x, y, dir, 1.0, 1, world),travel(0){};

void Squirt::doSomething()
{
    if (!isAlive())
        return;
    if (annoyProtesters() || travel == 4 )
    {
        setDead();
        return;
    }
    switch (getDirection())
    {
        case left:
            if (getWorld()->isEarth(getX() - 1, getY()) || getWorld()->isBoulder(getX() - 1, getY())) {
                setDead(); return;
            }
            else   moveTo(getX() - 1, getY());
            break;
        case right:
            if (getWorld()->isEarth(getX() + 1, getY()) || getWorld()->isBoulder(getX() + 1, getY())) {
                setDead(); return;
            }
            else   moveTo(getX() + 1, getY());
            break;
        case up:
            if (getWorld()->isEarth(getX(), getY() + 1) || getWorld()->isBoulder(getX(), getY() + 1)) {
                setDead(); return;
            }
            else moveTo(getX(), getY() + 1);
            break;
        case down:
            if (getWorld()->isEarth(getX(), getY() - 1) || getWorld()->isBoulder(getX(), getY() - 1)) {
                setDead(); return;
            }
            else moveTo(getX(), getY() - 1);
            break;
        case none : return;
    }
    travel++;
}

bool Squirt::annoyProtesters()
{
    Protester * p= getWorld()->protesterTooClose(this,3);
    if (p != nullptr) {
        p->isAnnoyed(2);
        return true;
    }
    return false;
}


//=================================== ITEM ===================================
Item::Item(int imageID, int x, int y, StudentWorld* world)
    :Actor(imageID, x, y, right, 1.0, 2, world){}

void Item::disapTimer(int ticks)
{
    if (tick == ticks)
        setDead();
    else
        tick++;
}

//=================================== BARREL ===================================
Barrel::Barrel(int x, int y, StudentWorld* world)
    :Item(TID_BARREL, x, y, world)
{
    setVisible(false);
}

void Barrel::doSomething()
{
    if (!isAlive())
        return;
    if (!isVisible() && getWorld()->playerTooClose(this, 4))
    {
        setVisible(true);
        return;
    }
    if (getWorld()->playerTooClose(this, 3))
    {
        setDead();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->decBarrel();
        return;
    }
}

//=================================== GOLD ===================================
Gold::Gold(StudentWorld* world, int x, int y, bool isVisible, bool pickupable)
    :Item(TID_GOLD, x, y, world), isDropped(pickupable)
{
    setVisible(isVisible);
}

void Gold::doSomething()
{
    if (!isAlive())
        return;
    if (!isVisible() && getWorld()->playerTooClose(this, 4))
    {
        setVisible(true);
        return;
    }
    if (!isDropped && getWorld()->playerTooClose(this, 3))
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->inc(getID());
        getWorld()->increaseScore(10);
        return;
    }
    if (isDropped)
    {
        Protester* p = (getWorld()->protesterTooClose(this, 3));
        if (p != nullptr)
        {
            setDead();
            p->beBribed();
        };
        disapTimer(100);
    }
}

//=================================== GOODIES ===================================
Goodie::Goodie(int imageID, int x, int y, StudentWorld* world)
    :Item(imageID, x, y, world)
{

}

void Goodie::doSomething(){
    if (!isAlive()) return;
    if (getWorld()->playerTooClose(this, 3)) {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->inc(getID());
        getWorld()->increaseScore(75);
        return;
    }
    disapTimer(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

//=================================== SONAR ===================================
Sonar::Sonar(StudentWorld* world, int x, int y)
    :Goodie(TID_SONAR, x, y, world) {}


//=================================== WATERPOOL ===================================
WaterPool::WaterPool(StudentWorld* world, int x, int y)
    :Goodie(TID_WATER_POOL, x, y, world) {}



//=================================== PROTESTER CLASS ===================================
Protester::Protester(StudentWorld* world, int imageID, int hitpoint)
    : Human(world, imageID, 60, 60, left, hitpoint), leaveField(false), tickcnt(200), checkYell(15)
{
    numSquaresToMoveInCurrentDirection();
    waitTimer = max(0, 3 - (int)getWorld()->getLevel() / 4);
}

void Protester::doSomething()
{
    if (!isAlive())
        return;
        
    if (waitTimer > 0)
    {
        waitTimer--;
        return;
    }
    else
    {
        waitTimer = max(0, 3 - (int)getWorld()->getLevel() / 4);
        tickcnt++;
        checkYell++;
    }
    
    if (leaveField)
    {
        if (getX()==60 && getX()==60)
        {
            getWorld()->decProtester();
            setDead();
            return;
        }
        getWorld()->moveToExit(this);
        return;
    }

    if (getWorld()->playerTooClose(this, 4) && checkFace()) // shout
    {
        if (checkYell>15)
        {
            getWorld()->getPlayer()->isAnnoyed(2);
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            checkYell = 0;
            return;
        }
        return;
    }
    if (getID()==TID_HARD_CORE_PROTESTER)
    {
        int M = 16+int(getWorld()->getLevel());
        Direction s = getWorld()->moveToPlayer(this, M);
        if(s!=none)
        {
            moveInDirection(s);
            return;
        }
    }

    Direction d = playerPos();
    if (d!=none && straightPath(d) && (!getWorld()->playerTooClose(this, 4)))
    {
        setDirection(d);
        moveInDirection(d);
        numSquares=0;
        return;
    }
    
    numSquares--;
    if (numSquares <= 0)
    {
        Direction k = none;
        k = randomDirection();
        while (true)
        {
            k = randomDirection();
            if (getWorld()->isWalkable(getX(),getY(), k)) break;
        }
        setDirection(k);
        numSquaresToMoveInCurrentDirection();
    }
    
    else if (isAtIntersection() && tickcnt > 200)
    {
        turnAround();
        tickcnt = 0;
        numSquaresToMoveInCurrentDirection();
    }
    
    moveInDirection(getDirection());
    if (!getWorld()->isWalkable(getX(),getY(),getDirection())) {
        numSquares = 0;
    }
}



GraphObject::Direction Protester::playerPos()
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();
    if (getY() == playerY && getX() == playerX)
        return getDirection();
    if (getX() == playerX)
    {
        if (getY() < playerY)
            return up;
        if (getY() > playerY)
            return down;
    }
    if (getY() == playerY)
    {
        if (getX() > playerX)
            return left;
        if (getY() < playerY)
            return right;
    }
    
    return none;
}

bool Protester::straightPath(Direction direction)
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getX();
    switch (direction) {
    case left:
        for (int i = getX(); i >= playerX; i--) {
            if (getWorld()->isEarth(i, getY()) || getWorld()->isBoulder(i, getY()))
                return false;
        }
        return true;
        break;
    case right:
        for (int i = getX(); i <= playerX; i++) {
            if (getWorld()->isEarth(i, getY()) || getWorld()->isBoulder(i, getY()))
                return false;
        }
        return true;
        break;
    case up:
        for (int j = getY(); j <= playerY; j++) {
            if (getWorld()->isEarth(getX(), j) || getWorld()->isBoulder(getX(), j))
                return false;
        }
        return true;
        break;
    case down:
        for (int j = getY(); j >= playerY; j--) {
            if (getWorld()->isEarth(getX(), j) || getWorld()->isBoulder(getX(), j))
                return false;
        }
        return true;
        break;
    case none:
        return false;
    }
}

GraphObject::Direction Protester::randomDirection()
{
    int r;
    r = rand() % 4;
    switch (r)
    {
        case 0: return left;
        case 1: return  right;
        case 2: return up;
        case 3: return down;
    }
    return none;
}

bool Protester::isAtIntersection()
{
    if (getDirection() == up || getDirection() == down)
        return (getWorld()->isWalkable(getX(),getY(), left) || getWorld()->isWalkable(getX(),getY(), right));
    return (getWorld()->isWalkable(getX(),getY(), up) || getWorld()->isWalkable(getX(),getY(), down));
}

void Protester::turnAround()
{
    if (getDirection() == up || getDirection() == down)
    {
        if (!getWorld()->isWalkable(getX(),getY(), left))
            setDirection(right);
        else if (!getWorld()->isWalkable(getX(),getY(), right))
            setDirection(left);
        else
        {
            switch (rand() % 2)
            {
                case 0:
                    setDirection(left);
                case 1:
                    setDirection(right);
            }
        }
    }
    else
    {
        if (!getWorld()->isWalkable(getX(),getY(), up))
            setDirection(down);
        else if (!getWorld()->isWalkable(getX(),getY(), down))
            setDirection(up);
        else
        {
            switch (rand() % 2)
            {
                case 0:
                    setDirection(up);
                case 1:
                    setDirection(down);
            }
        }
    }

}

void Protester::isAnnoyed(int hitpoint)
{
    if (leaveField)
        return;
    setHealth(hitpoint);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    beStunned();
    if (getHealth()<=0)
    {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        leaveField = true;
        waitTimer=0;
        if (hitpoint==100)
            getWorld()->increaseScore(500); // bonked by a boulder
        else if (getID()==TID_PROTESTER)
            getWorld()->increaseScore(100);
        else
            getWorld()->increaseScore(250);
    }
}

void Protester::beStunned()
{
    waitTimer = max(50, 100 - (int)getWorld()->getLevel()*10);
}

void Protester::beBribed()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    if(getID()==TID_PROTESTER)
    {
        getWorld()->increaseScore(25);
        leaveField = true;
    }
    else {
        getWorld()->increaseScore(50);
        waitTimer = max(50,100- int(getWorld()->getLevel())*10);
    }
}

bool Protester::checkFace()
{
    switch (getDirection()) {
        case left:
            return getWorld()->getPlayer()->getX() <= getX();
        case right:
            return getWorld()->getPlayer()->getX() >= getX();
        case up:
            return getWorld()->getPlayer()->getY() >= getY();
        case down:
            return getWorld()->getPlayer()->getY() <= getY();
        case none:
            return false;
    }
    return false;
}

//RegularProtester
RegularProtester::RegularProtester(StudentWorld* world)
    : Protester(world, TID_PROTESTER, 5) {}



//HardcoreProtester
HardcoreProtester::HardcoreProtester(StudentWorld* world)
    : Protester(world, TID_HARD_CORE_PROTESTER, 20) {}




