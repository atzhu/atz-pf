#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <queue>
#include <cstdlib>
#include <vector>
using namespace std;

class StudentWorld;

/*Constants used to help change test parameters for testing purposes*/
const int MAX_SPRAYERS          = 2;
const int SIMPLE_ZUMI_SCORE     = 100;
const int COMPLEX_ZUMI_SCORE    = 500;
const int GOODIE_SCORE          = 1000;
const int BUGSPRAYER_LIFETIME   = 40;
const int BUGSPRAY_LIFETIME     = 3;
const int BUGSPRAY_SPREAD       = 2;
const int SIMPLE_MOVE_LEFT      = 0;
const int SIMPLE_MOVE_RIGHT     = 1;
const int SIMPLE_MOVE_UP        = 2;
const int SIMPLE_MOVE_DOWN      = 3;

/*Coord class is used to store an x and y value in a single object
 *Primarily used for the complexMove function for complex Zumis
 */
class Coord
{
public:
    Coord(int x, int y): m_x(x), m_y(y){}
    int x() const{return m_x;}
    int y() const{return m_y;}
private:
    int m_x;
    int m_y;
};

/*Actor class, abstract base class*/
class Actor : public GraphObject
{
public:
    Actor(int imageID, int x, int y, StudentWorld& World): GraphObject(imageID, x, y), localWorld(World), liveStatus(true) {setVisible(true);}
    virtual int imageID() const=0;              // Pure virtual, the derived classes provide the imageID
    virtual void doSomething()=0;               // Pure virtual, each derived class does its own thing
    virtual void deadBehavior(){return;}        // Does nothing by default, only used by Zumi objects
    bool isAlive() const{return liveStatus;}
    void changeLiveStatus(){if(isAlive()) liveStatus = !liveStatus;}
    StudentWorld& getWorld() const{return localWorld;}
private:
    StudentWorld& localWorld;
    bool liveStatus;
};

class Brick : public Actor
{
public:
    Brick(int imageID, int x, int y, StudentWorld& World): Actor(imageID, x, y, World){}
    virtual void doSomething(){return;}         // Bricks do nothing
};

class PermanentBrick : public Brick
{
public:
    PermanentBrick(int x, int y, StudentWorld& World): Brick(imageID(), x, y, World){}
    virtual int imageID() const{return IID_PERMA_BRICK;}
};

class DestroyableBrick : public Brick
{
public:
    DestroyableBrick(int x, int y, StudentWorld& World): Brick(imageID(), x, y, World){}
    virtual int imageID() const{return IID_DESTROYABLE_BRICK;}
};

class Player : public Actor
{
public:
    Player(int x, int y, StudentWorld& World): Actor(imageID(), x, y, World), extraLife(false), canWalkThru(false), haveIncSprayers(false), ISlifetime(0), WTlifetime(0){}
    virtual int imageID() const{return IID_PLAYER;}
    virtual void doSomething();
    void addExtraLife();
    void walkThruWalls(int n);                      // When picking up the walk through walls goodie
    void increaseSprayers(int n);               // When picking up the increase sprayers goodie
    bool canMoveTo(int x, int y);               // Checks to see if the player can be on a certain coordinate
private:
    bool extraLife;                             // Check to see if the goodie has been activated
    bool canWalkThru;                           // Check to see if the goodie is active
    bool haveIncSprayers;                       // Check to see if the goodie is active
    int ISlifetime;                             // Lifetime of the goodie
    int WTlifetime;                             // Lifetime of the goodie
};

class Zumi : public Actor
{
public:
    Zumi(int imageID, int x, int y, StudentWorld& World, int n): Actor(imageID, x, y, World), maxNumTicks(n){currentDir = rand()%4;}
    bool canMove(int& n);                       // Checks to see if it can move based on the tick delay
    bool canMoveTo(int x, int y);               // Checks to see if it can be on a certain coordinate
    void checkPlayerPos(Player* p);             // Checks to see if it is on the same spot as a player
    virtual void deadBehavior();                // Zumis have a chance to drop a goodie upon dying
    void changeDir(){currentDir = rand()%4;}    // Zumis can change to a random direction
    void simpleMove();                          // "Simple" movement of the simple Zumi
private:
    int currentDir;
    const int maxNumTicks;
};

class SimpleZumi : public Zumi
{
public:
    SimpleZumi(int x, int y, StudentWorld& World, int N): Zumi(imageID(), x, y, World, N), numTicks(N){}
    virtual int imageID() const{return IID_SIMPLE_ZUMI;}
    virtual void doSomething();
private:
    int numTicks;
};

class ComplexZumi : public Zumi
{
public:
    ComplexZumi(int x, int y, StudentWorld& World, int n): Zumi(imageID(), x, y, World, n), numTicks(n){rootptr = nullptr;}
    virtual int imageID() const{return IID_COMPLEX_ZUMI;}
    virtual void doSomething();
    int horizDistFromPlayer();                  // Horizontal distance from player
    int vertDistFromPlayer();                   // Vertical distance from player
    void complexMove();                         // "Complex" movement of the complex Zumi
    /*The function coordChecked checks to see if the given x and y values already exist in a coordinate that is in the vector
     *This is used for the breadth-first search in the complexMove function
     */
    bool coordChecked(int x, int y, vector<Coord> Coords);
private:
    int numTicks;
    /*This node structure is used to create a tree that maps the complexMove function
     *Nodes are used so that the shortest path between the complex Zumi and Player can be traced
     */
    struct Node
    {
        Node(int x, int y): m_x(x), m_y(y){}
        ~Node(){}
        int m_x;
        int m_y;
        Node* parent;                           // Points to previous node, or where the last "step" was
        Node* child[4];                         // Points to the 4 possible directions from the current coordinate
    };
    Node* rootptr;                              // Points to the first coordinate, where the Zumi is
};

class BugSprayer : public Actor
{
public:
    BugSprayer(int x, int y, StudentWorld& World): Actor(imageID(), x, y, World), lifetime(BUGSPRAYER_LIFETIME){}
    virtual int imageID() const{return IID_BUGSPRAYER;}
    virtual void doSomething();
    void setLifetimeToZero(){lifetime = 0;}     // In the instance that a BugSprayer is triggered by nearby Spray
private:
    int lifetime;
};

class BugSpray : public Actor
{
public:
    BugSpray(int x, int y, StudentWorld& World): Actor(imageID(), x, y, World), lifetime(BUGSPRAY_LIFETIME){}
    virtual int imageID() const{return IID_BUGSPRAY;}
    virtual void doSomething();
    /*The function SpraySpread is what creates the + explosion from the BugSprayer
     *This function checks to see if any nearby Bricks block the explosion or get erased
     */
    void SpraySpread(int x, int y, StudentWorld& World, bool& cont);
private:
    int lifetime;
};

class Goodies : public Actor
{
public:
    Goodies(int imageID, int x, int y, StudentWorld& World, int n): Actor(imageID, x, y, World), lifetime(n), activated(false){}
    void decLifetime();                         // Decrease the amount of time the goodie is "alive"
    bool isActivated(){return activated;}       // Checks to see if the goodie has been activated
    /*The function checkPlayer does two things
     *First it checks to see if the player is on the same spot as the goodie
     *Then it activates the goodie, increases the score, plays the sound, and deletes the goodie object
     */
    void checkPlayer(int x, int y, StudentWorld& World);
private:
    int lifetime;
    bool activated;
};

class ExtraLifeGoodie : public Goodies
{
public:
    ExtraLifeGoodie(int x, int y, StudentWorld& World, int n): Goodies(imageID(), x, y, World, n){}
    virtual int imageID() const{return IID_EXTRA_LIFE_GOODIE;}
    virtual void doSomething();
};

class WalkThruGoodie : public Goodies
{
public:
    WalkThruGoodie(int x, int y, StudentWorld& World, int n, int l): Goodies(imageID(), x, y, World, n), lifetime(l){}
    virtual int imageID() const{return IID_WALK_THRU_GOODIE;}
    virtual void doSomething();
private:
    int lifetime;                               // Number of ticks the Player is allowed to walk through walls
};

class IncSprayerGoodie : public Goodies
{
public:
    IncSprayerGoodie(int x, int y, StudentWorld& World, int n, int l): Goodies(imageID(), x, y, World, n), lifetime(l){}
    virtual int imageID() const{return IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE;}
    virtual void doSomething();
private:
    int lifetime;                               // Number of ticks the Player has increased BugSprays
};

class Exit : public Actor
{
public:
    Exit(int x, int y, StudentWorld& World): Actor(imageID(), x, y, World){setVisible(false);}
    virtual int imageID() const{return IID_EXIT;}
    virtual void doSomething();
};

#endif // ACTOR_H_
