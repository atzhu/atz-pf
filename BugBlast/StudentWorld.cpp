#include "StudentWorld.h"

GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << getLevel() << ".dat";
    Level lev;
    Level::LoadResult result = lev.loadLevel(oss.str());
    
    /*Level technicalities*/
    if(getLevel() == 0 && result == Level::load_fail_file_not_found)
    {
        return GWSTATUS_NO_FIRST_LEVEL;
    }
    else if(result == Level::load_fail_file_not_found)
    {
        return GWSTATUS_PLAYER_WON;
    }
    else if(result == Level::load_fail_bad_format)
    {
        return GWSTATUS_LEVEL_ERROR;
    }
    
    /*Load all of the level parameters
     *The private data members are really initialized here, rather than by the constructor
     */
    levelBonus = lev.getOptionValue(optionLevelBonus);
    goodieLife = lev.getOptionValue(optionGoodieLifetimeInTicks);
    goodieProb = lev.getOptionValue(optionProbOfGoodieOverall);
    ELGoodieProb = lev.getOptionValue(optionProbOfExtraLifeGoodie);
    WTGoodieProb = lev.getOptionValue(optionProbOfWalkThruGoodie);
    ISGoodieProb = lev.getOptionValue(optionProbOfMoreSprayersGoodie);
    boostedSprayNum = lev.getOptionValue(optionMaxBoostedSprayers);
    boostedSprayLife = lev.getOptionValue(optionBoostedSprayerLifetimeTicks);
    walkThruWallsLife = lev.getOptionValue(optionWalkThruLifetimeTicks);
    cZumiSmellDist = lev.getOptionValue(optionComplexZumiSearchDistance);
    
    /*Initializes the level by adding newly created objects to the container*/
    for(int i = 0; i < VIEW_HEIGHT; i++)
    {
        for(int j = 0; j < VIEW_WIDTH; j++)
        {
            Level::MazeEntry ge = lev.getContentsOf(i,j);
            switch (ge)
            {
                case Level::empty:
                    break;
                case Level::player:
                    Player = new Player::Player(i, j, *this);
                    break;
                case Level::simple_zumi:
                    Objects.push_back(new SimpleZumi::SimpleZumi(i, j, *this, lev.getOptionValue(optionTicksPerSimpleZumiMove)));
                    break;
                case Level::complex_zumi:
                    Objects.push_back(new ComplexZumi::ComplexZumi(i, j, *this, lev.getOptionValue(optionTicksPerComplexZumiMove)));
                    break;
                case Level::exit:
                    Objects.push_back(new Exit::Exit(i, j, *this));
                    break;
                case Level::perma_brick:
                        Objects.push_back(new PermanentBrick::PermanentBrick(i, j, *this));
                    break;
                case Level::destroyable_brick:
                    Objects.push_back(new DestroyableBrick::DestroyableBrick(i, j, *this));
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    /*Set the game text*/
    ostringstream o_score;
    o_score.fill('0');
    o_score << "Score: " << setw(8) << getScore();
    ostringstream o_level;
    o_level.fill('0');
    o_level << " Level: " << setw(2) << getLevel();
    ostringstream o_lives;
    o_lives.fill('0');
    o_lives << " Lives: " << setw(3) << getLives();
    ostringstream o_bonus;
    o_bonus.fill(' ');
    o_bonus << " Bonus: " << setw(6) << levelBonus;
    string s = o_score.str() + o_level.str() + o_lives.str() + o_bonus.str();
    setGameStatText(s);
    
    /*Players and Object both doSomething*/
    Player->doSomething();
    for(int i = 0; i < Objects.size(); i++)
    {
        if(Objects[i]->isAlive())
        {
            Objects[i]->doSomething();
        }
        if(!Objects[i]->isAlive())
        {
            Objects[i]->deadBehavior();
            delete Objects[i];
            Objects.erase(Objects.begin()+i);
            i--;
        }
    }
    if(levelBonus > 0) levelBonus--;                // Decrease the level bonus every tick
    /*No more Zumis are left in the level
     *If the exit has not been found yet, then it will be revealed
     *Otherwise nothing else happens
     */
    if(numberOfActors(IID_SIMPLE_ZUMI) == 0 && numberOfActors(IID_COMPLEX_ZUMI) == 0)
    {
        if(!exitFound)
        {
            playSound(SOUND_REVEAL_EXIT);
            for(int i = 0; i < Objects.size(); i++)
            {
                if(Objects[i]->getID() == IID_EXIT)
                {
                    Objects[i]->setVisible(true);
                }
            }
            exitFound = true;
        }
    }
    if (!Player->isAlive())                         // Player has died during this tick
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();                                 // Decrement lives
        return GWSTATUS_PLAYER_DIED;
    }
    if(levelCompleted)                              // Found exit has been stepped on by Player
    {
        increaseScore(levelBonus);                  // Add bonus to score
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete Player;                                  // Delete the player
    for(int i = 0; i < Objects.size(); i++)         // Delete all objects in the container
    {
        delete Objects[i];
    }
    Objects.clear();                                // Clear the container
    exitFound = false;                              // Reset exitFound
    levelCompleted = false;                         // Reset levelCompleted
}

vector<Actor*> StudentWorld::getActors(int x, int y)
{
    vector<Actor*> actorsOnCoord;
    for(int i = 0; i < Objects.size(); i++)
    {
        if(x == Objects[i]->getX() &&               // Same location as arguments
           y == Objects[i]->getY())
        {
            actorsOnCoord.push_back(Objects[i]);
        }
    }
    return actorsOnCoord;
}

int StudentWorld::typeOfActor(int x, int y) const
{
    for(int i = 0; i < Objects.size(); i++)
    {
        if(x == Objects[i]->getX() &&               // Same location as arguments
           y == Objects[i]->getY())
        {
            return Objects[i]->getID();             // Return the ID of that object
        }
    }
    return -999;
}

int StudentWorld::numberOfActors(const int ID) const
{
    int numActors = 0;
    for(int i = 0; i < Objects.size(); i++)
    {
        if(Objects[i]->getID() == ID) numActors++;  // Increment every time the ID matches
    }
    return numActors;                               // Return the number of Actors of ID argument
}