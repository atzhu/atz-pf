#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(): exitFound(false), levelCompleted(false), levelBonus(0), maxNumberSprayers(2), goodieLife(0), goodieProb(0), ELGoodieProb(0), WTGoodieProb(0), ISGoodieProb(0), boostedSprayNum(0), boostedSprayLife(0), walkThruWallsLife(0), cZumiSmellDist(0){};
    ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
    
    /*Pointers to Actors, including Player*/
    Player* getPlayer(){return Player;}
    vector<Actor*> getActors(int x, int y);                     // Vector of all the Actors on a certain coordinate
    
    int typeOfActor(int x, int y) const;
    void addToContainer(Actor* a){Objects.push_back(a);}
    int numberOfActors(const int ID) const;
    
    /*Functions that get the level parameters set by the .dat file*/
    int getMaxSprayers() const{return maxNumberSprayers;}
    int getGoodieLife() const{return goodieLife;}
    int getGoodieProb() const{return goodieProb;}
    int getELGoodieProb() const{return ELGoodieProb;}
    int getWTGoodieProb() const{return WTGoodieProb;}
    int getISGoodieProb() const{return ISGoodieProb;}
    int getBoostedSprayNum() const{return boostedSprayNum;}
    int getBoostedSprayLife() const{return boostedSprayLife;}
    int getWalkThruLife() const{return walkThruWallsLife;}
    int getCZumiSmellDist() const{return cZumiSmellDist;}
    
    void setMaxSprayers(int max){maxNumberSprayers = max;}
    bool isExitFound(){return exitFound;}
    void complete(){levelCompleted = true;}
    
private:
    /*Object container and Player pointer*/
    vector<Actor*> Objects;
    Player* Player;
    
    bool exitFound;
    bool levelCompleted;
    unsigned int levelBonus;
    
    /*Level parameters*/
    int maxNumberSprayers;
    int goodieLife;
    int goodieProb;
    int ELGoodieProb;
    int WTGoodieProb;
    int ISGoodieProb;
    int boostedSprayNum;
    int boostedSprayLife;
    int walkThruWallsLife;
    int cZumiSmellDist;

};

#endif // STUDENTWORLD_H_
