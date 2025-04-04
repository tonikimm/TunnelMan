#ifndef ACTOR_H_
#define ACTOR_H_
#include "GameConstants.h"
#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

//=================================== ACTOR BASE CLASS ===================================
class Actor: public GraphObject
{
public:
    //constructor & destructor
    Actor(int imageID, int x, int y, Direction dir, double size, unsigned int depth, StudentWorld* studentworld);
    virtual ~Actor();

    //accessors
    bool isAlive() const { return isalive; }
    StudentWorld* getWorld()const { return world; }

    //mutators
    void setDead() { isalive=false; }

    //member functions
    virtual void doSomething() = 0;
    void moveTo(int x, int y);
    
private:
    StudentWorld* world;
    bool isalive;

};


//=================================== HUMAN CLASS ===================================
class Human: public Actor
{
public:
    //constructor & destructor
    Human(StudentWorld* world, int imageID, int x, int y, Direction dir, int hitpoint);
    virtual ~Human() {}
    
    //accessors
    int getHealth() const { return health; }
    
    //mutators
    void setHealth(int amt) { health-=amt; }

    //member functions
    void moveInDirection(Direction direction);
    virtual void isAnnoyed(int hitpoint) = 0;
    
private:
    int health;
};


//=================================== TUNNELMAN ===================================
class TunnelMan: public Human
{
    public:
        TunnelMan(StudentWorld* world);
        virtual ~TunnelMan() {}
        virtual void doSomething();

        void inc(int id);
        int getSquirts() const { return squirts; }
        int getSonar() const { return sonar; }
        int getGold() const { return gold; }

        virtual void isAnnoyed(int hitpoint);
        void shoot();

    private:
        int squirts;
        int sonar;
        int gold;
};


//=================================== EARTH ===================================
class Earth: public Actor
{
    public:
        Earth(int x, int y, StudentWorld* world);
        virtual ~Earth() {}
        virtual void doSomething() {}
};


//=================================== BOULDER ===================================
class Boulder: public Actor
{
    public:
        Boulder(int x, int y, StudentWorld* world);
        virtual void doSomething();
        void hitMan();
    private:
        bool stable;
        bool isFalling;
        int ticks;
        bool haveHit = false;

};


//=================================== SQUIRT ===================================
class Squirt: public Actor
{
    public:
        Squirt(StudentWorld* world, int x, int y, Direction dir);
        virtual void doSomething();
        bool annoyProtesters();
    private:
        int travel;
};


//=================================== ITEM CLASS ===================================
class Item: public Actor
{
    public:
        Item(int imageID, int x, int y, StudentWorld* world);
        virtual void doSomething() = 0;
        void disapTimer(int ticks);
    private:
        int tick;
};


//=================================== BARREL ===================================
class Barrel : public Item
{
    public:
        Barrel(int x, int y, StudentWorld* world);
        virtual void doSomething();
};

//=================================== GOLD ===================================
class Gold :public Item
{
    public:
        Gold(StudentWorld* world, int x, int y, bool visible,bool pickupable);
        virtual void doSomething();
    private:
        bool isDropped;
};

//=================================== GOODIE CLASS ===================================
class Goodie: public Item
{
    public:
        Goodie(int imageID, int x, int y, StudentWorld* world);
        virtual void doSomething();
};

//=================================== SONAR ===================================
class Sonar: public Goodie
{
    public:
        Sonar(StudentWorld* world, int x, int y);
};


//=================================== WATERPOOL ===================================
class WaterPool: public Goodie
{
    public:
        WaterPool(StudentWorld* world, int x, int y);
};


//=================================== PROTESTER CLASS ===================================
class Protester: public Human
{
    public:
        Protester(StudentWorld* world, int imageID,int hitpoint);
        virtual ~Protester() {}
        void doSomething();

        void beBribed();
        void isAnnoyed(int hitpoint);
        void beStunned();

    private:
        bool leaveField;
        int numSquares;
        int tickcnt;
        int nonRestTicks;
        int annoyance;
        int checkYell;
        int waitTimer;
        void numSquaresToMoveInCurrentDirection() { numSquares = rand() % 53 + 8; }
        void turnAround();
        Direction randomDirection();
        Direction playerPos();
        bool isAtIntersection();
        bool checkFace();
        bool straightPath(Direction direction);
};


//=================================== REGULAR PROTESTOR ===================================
class RegularProtester: public Protester
{
    public:
        RegularProtester(StudentWorld* world);
};


//=================================== HARDCORE PROTESTER ===================================
class HardcoreProtester: public Protester
{
    public:
        HardcoreProtester(StudentWorld* world);
};


#endif // ACTOR_H_
