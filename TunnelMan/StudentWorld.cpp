#include "StudentWorld.h"
#include "Actor.h"
#include "GraphObject.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}


//constructor
StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir),isFirstTick(true), ticksPassed(0), protestersAlive(0), player(nullptr), barrelsLeft(0)
{

}


//destructor
StudentWorld::~StudentWorld()
{
    cleanUp();
}


//================================= MEMBER FUNCTIONS =================================
TunnelMan * StudentWorld::getPlayer()
{
    return player;
}


string StudentWorld::displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels)
{
    stringstream s;

    s.fill('0');
    s << "Scr: ";
    s << setw(6) << score;

    s.fill(' ');
    s << " Lvl: ";
    s << setw(2) << level;

    s << " Lives: ";
    s << setw(1) << lives;

    s << "  Hlth: ";
    s << setw(3) << health * 10;
    s << '%';

    s << "  Wtr: ";
    s << setw(2) << squirts;

    s << "  Gld: ";
    s << setw(2) << gold;

    s << "  Sonar: ";
    s << setw(2) << sonar;

    s << "  Oil Left: ";
    s << setw(2) << barrels;

    return s.str();
}


void StudentWorld::updateText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = player->getHealth();
    int squirts = player->getSquirts();
    int gold = player->getGold();
    int sonar = player->getSonar();

    string s = displayText(score, level, lives, health, squirts, gold, sonar, barrelsLeft);
    setGameStatText(s);
}


bool StudentWorld::removeDirt(int x, int y) {
    bool clear = false;
    for (int i = x; i < x+4; i++)
        for (int j = y; j < y+4; j++)
        {
            if (earth[i][j] != nullptr) {
                delete earth[i][j];
                earth[i][j] = nullptr;
                clear = true;
            }
        }
    return clear;
}


void StudentWorld::addGoodies()
{
    int x, y;
    int G = (int)getLevel()*25 + 300;
    if (int(rand() % G) + 1 == 1) {
        if (int(rand() % 5) + 1 == 1) {
            addActor(new Sonar(this, 0, 60));
        }
        else {
            do {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            } while (isEarth(x, y));
            addActor(new WaterPool(this, x, y));
        }
    }
}


void StudentWorld::addProtesters()
{
    int T = max(25, 200 - (int)getLevel());
    int P = fmin(15, 2 + getLevel() * 1.5);
    int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
    if (isFirstTick || (ticksPassed > T && protestersAlive < P)) {
        if (rand() % 100 + 1 < probabilityOfHardcore)
            addActor(new HardcoreProtester(this));
        else addActor(new RegularProtester(this));
        ticksPassed = 0;
        protestersAlive++;
        isFirstTick = false;
    }
    ticksPassed++;
}


bool StudentWorld::withinRadius(int x1, int y1, int x2, int y2, int radius)
{
    if ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) <= radius * radius)
        return true;
    else return false;
}


bool StudentWorld::actorTooClose(int x, int y, int radius)
{
    vector<Actor*>::iterator it;
    it = actors.begin();
    while (it != actors.end())
    {
        if (withinRadius(x,y, (*it)->getX(),(*it)->getY() ,radius))
            return true;
        it++;
    }
    return false;
}

void StudentWorld::addItem(int num, int actor)
{
    int x, y;
    for (int i = 0; i < num; i++)
    {
        do {
            x = rand() % 60 + 1;
            if (actor == 1)
                y = rand() % 36 + 1 + 20;
            else
                y = rand() % 56 + 1;
        } while (actorTooClose(x, y, 6) || (x > 26 && x < 34 && y>0));
        switch (actor)
        {
            case 1:
                addActor(new Boulder(x,y,this));
                break;
            case 2:
                addActor(new Barrel(x,y,this));
                barrelsLeft++;
                break;
            case 3:
                addActor(new Gold(this, x, y,false,false));
                break;
        }
    }
}
int StudentWorld::init()
{
    barrelsLeft = 0;
    isFirstTick = true;
    ticksPassed = 0;
    protestersAlive = 0;

    for (int x = 0; x< 64 ; x++)
    {
        for (int y = 0;  y<60; y++)
        {
            if(x<30||x>33||y<4)
            earth[x][y] = new Earth(x,y,this);
        }
    }
    player = new TunnelMan(this);
    int B = min( (int)getLevel()/2 + 2, 9);
    int G = max((int)getLevel()/2, 2);
    int L = min( 2 + (int)getLevel(), 21);
    addItem(B, 1);
    addItem(L, 2);
    addItem(G, 3);

    return GWSTATUS_CONTINUE_GAME;
}


bool StudentWorld::isSupported(int x, int y)
{
    for (int i = x; i < x + 4; i++)
    {
        if (earth[i][y] != nullptr)
            return true;
    }
    return false;
}
bool StudentWorld::isEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++) {

        for (int j = y; j <y + 4; j++) {

            if (earth[i][j] != nullptr)
                return true;
        }
    }
    return false;
}
bool StudentWorld::isBoulder(int x, int y,int radius)
{
    vector<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++) {
        if ((*it)->getID() == TID_BOULDER && withinRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
            return true;
    }
    return false;
}
bool StudentWorld::isWalkable(int x, int y, GraphObject::Direction direction)
{
    switch (direction) {
    case GraphObject::left:
        return (x!=0&&!isEarth(x - 1, y) && !isBoulder(x, y));
    case GraphObject::right:
        return (x!=60&& !isEarth(x + 1, y) && !isBoulder(x + 1, y));
    case GraphObject::up:
        return (y!=60&&!isEarth(x, y + 1) && !isBoulder(x, y + 1));
    case GraphObject::down:
        return (y!=0&& !isEarth(x, y - 1) && !isBoulder(x, y - 1));
    case GraphObject::none:
            return false;
    }
    return false;
}
bool StudentWorld::playerTooClose(Actor *actor, int radius)
{
    return withinRadius(actor->getX(), actor->getY(), player->getX(), player->getY(), radius);
}

Protester* StudentWorld::protesterTooClose(Actor * actor, int radius)
{
    vector<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++) {
        if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
            if (withinRadius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius))
                return dynamic_cast<Protester*> (*it);
    }
    return nullptr;
}

void StudentWorld::moveToExit(Protester * pr)
{
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            m_maze[i][j]=0;
        }
    }
    int a =pr->getX();
    int b =pr->getY();
    queue<Grid> q;
    q.push(Grid(60,60));
    m_maze[60][60]=1;
    while (!q.empty()) {
        Grid c = q.front();
        q.pop();
        int x=c.x;
        int y=c.y;

        //left
        if(isWalkable(x,y, GraphObject::left)&& m_maze[x-1][y]==0){
            q.push(Grid(x-1,y));
            m_maze[x-1][y] =m_maze[x][y]+1;
        }
        //right
        if(isWalkable(x,y, GraphObject::right)&& m_maze[x+1][y]==0){
            q.push(Grid(x+1,y));
            m_maze[x+1][y] =m_maze[x][y]+1;
        }
        //up
        if(isWalkable(x,y, GraphObject::up)&& m_maze[x][y+1]==0){
            q.push(Grid(x,y+1));
            m_maze[x][y+1] =m_maze[x][y]+1;
        }
        // down
        if(isWalkable(x,y, GraphObject::down)&& m_maze[x][y-1]==0){
            q.push(Grid(x,y-1));
            m_maze[x][y-1] =m_maze[x][y]+1;
        }
    }
    if(isWalkable(a,b, GraphObject::left)&& m_maze[a-1][b]<m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::left);
    //    cout<<"L"<<" "<<a-1<<","<<b<<" "<<pr->getX()<<","<<pr->getY()<<endl;
        return;
    }
    if(isWalkable(a,b, GraphObject::right)&& m_maze[a+1][b]<m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::right);
    //    cout<<"R"<<" "<<a+1<<","<<b<<" "<<pr->getX()<<","<<pr->getY()<<endl;
        return;
    }
    if(isWalkable(a,b, GraphObject::up)&& m_maze[a][b+1]<m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::up);
    //    cout<<"U"<<" "<<a<<","<<b+1<<" "<<pr->getX()<<","<<pr->getY()<<endl;
        return;
    }
    if(isWalkable(a,b, GraphObject::down)&&m_maze[a][b-1]<m_maze[a][b])
    {
        pr->moveInDirection(GraphObject::down);
    //    cout<<"D"<<" "<<a<<","<<b-1<<" "<<pr->getX()<<","<<pr->getY()<<endl;
    }
    return;
}

void StudentWorld::detectNearActors(int x, int y, int radius)
{
    int a, b;
    vector<Actor*>::iterator it;
    for (it = actors.begin(); it != actors.end(); it++)
    {
        if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
        {
            a = (*it)->getX(); b = (*it)->getY();
            if ((x - a)*(x - a) + (y - b)*(y - b) <= radius * radius)
                (*it)->setVisible(true);
        }
    }
}

GraphObject::Direction StudentWorld::moveToPlayer(Protester * pr, int M)
{
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            m_maze[i][j]=0;
        }
    }
    int a =pr->getX();
    int b =pr->getY();
    queue<Grid> q;
    q.push(Grid(getPlayer()->getX(),getPlayer()->getY()));
    m_maze[getPlayer()->getX()][getPlayer()->getY()]=1;
    while (!q.empty())
    {
        Grid c = q.front();
        q.pop();
        int x=c.x;
        int y=c.y;
        
        //left
        if(isWalkable(x,y, GraphObject::left)&& m_maze[x-1][y]==0){
            q.push(Grid(x-1,y));
            m_maze[x-1][y] =m_maze[x][y]+1;
        }
        //right
        if(isWalkable(x,y, GraphObject::right)&& m_maze[x+1][y]==0){
            q.push(Grid(x+1,y));
            m_maze[x+1][y] =m_maze[x][y]+1;
        }
        //up
        if(isWalkable(x,y, GraphObject::up)&& m_maze[x][y+1]==0){
            q.push(Grid(x,y+1));
            m_maze[x][y+1] =m_maze[x][y]+1;
        }
        // down
        if(isWalkable(x,y, GraphObject::down)&& m_maze[x][y-1]==0){
            q.push(Grid(x,y-1));
            m_maze[x][y-1] =m_maze[x][y]+1;
        }
    }
    if(m_maze[a][b] <= M+1){
        if(isWalkable(a,b, GraphObject::left)&& m_maze[a-1][b]<m_maze[a][b])
            return GraphObject::left;
        if(isWalkable(a,b, GraphObject::right)&& m_maze[a+1][b]<m_maze[a][b])
            return GraphObject::right;
        if(isWalkable(a,b, GraphObject::up)&& m_maze[a][b+1]<m_maze[a][b])
            return GraphObject::up;
        if(isWalkable(a,b, GraphObject::down)&&m_maze[a][b-1]<m_maze[a][b])
            return GraphObject::down;
    }
    return GraphObject::none;
}


int StudentWorld::move()
{
    updateText();

    vector<Actor*>::iterator it;
    for (it = actors.begin(); it!=actors.end(); it++)
    {
        if ((*it)->isAlive()) {
            (*it)->doSomething();
        }
        if (!player->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (barrelsLeft == 0) {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    player->doSomething();

    addGoodies();
    addProtesters();

    for (it = actors.begin(); it != actors.end();)
    {
        if (!(*it)->isAlive())
        {
            delete *it;
            it = actors.erase(it);
        }
        else
            it++;
    }
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 60; y++)
        {
            delete earth[x][y];
        }
    }
    vector<Actor*>::iterator it;
    for (it = actors.begin();it != actors.end();it++)
    {
        delete *it;
    }
    actors.clear();
    delete player;

}
