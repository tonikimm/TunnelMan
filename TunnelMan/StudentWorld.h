#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <queue>
using namespace std;

class GraphObject;
class Earth;
class TunnelMan;
class Actor;
class Protester;


class StudentWorld : public GameWorld
{
    public:
        StudentWorld(std::string assetDir);
        ~StudentWorld();

        virtual int init();
        virtual int move();
        virtual void cleanUp();

        TunnelMan* getPlayer();

        string displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels);
        void updateText();

        void detectNearActors(int x, int y, int radius);
        bool playerTooClose(Actor* actor, int radius);
        Protester* protesterTooClose(Actor* actor, int radius);

        bool removeDirt(int x, int y);
        bool isSupported(int x, int y);
        bool isEarth(int x, int y);
        bool isBoulder(int x, int y,int radius=3);

        bool isWalkable(int x, int y, GraphObject::Direction direction);
        void moveToExit(Protester* p);
        GraphObject::Direction moveToPlayer(Protester* pr, int M);
        
        void addActor(Actor* actor) { actors.push_back(actor); }
        void decBarrel() { barrelsLeft--; }
        void decProtester() { protestersAlive--; }

    private:
        bool isFirstTick;
        int ticksPassed;
        int protestersAlive;
        int barrelsLeft;
        Earth* earth[64][64];
        
        TunnelMan* player;
        vector<Actor*> actors;
        
        int m_maze[64][64];
        struct Grid {
            int x;
            int y;
            Grid(int a, int b): x(a),y(b){}
        };
        bool withinRadius(int x1, int y1, int x2, int y2, int radius);
        void addProtesters();
        void addGoodies();
        void addItem(int num, int type);
        bool actorTooClose(int x, int y, int radius);
};

#endif // STUDENTWORLD_H_
