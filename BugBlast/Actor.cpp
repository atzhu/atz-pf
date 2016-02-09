#include "Actor.h"
#include "StudentWorld.h"

void Player::doSomething()
{
    if(!isAlive()) return;
    if(haveIncSprayers)
    {
        /*If the player is allowed to drop more BugSprayers, then this changes the max number
         *Decrements the lifetime of the Goodie effects every tick
         */
        getWorld().setMaxSprayers(getWorld().getBoostedSprayNum());
        ISlifetime--;
        if(ISlifetime == 0)
        {
            haveIncSprayers = false;                // Effect is no longer true
            getWorld().setMaxSprayers(MAX_SPRAYERS);// Resets the max number of sprayers
        }
    }
    if(canWalkThru)
    {
        /*If the player is allowed to walk through walls
         *Decrements the lifetime of the Goodie effects every tick
         */
        WTlifetime--;
        if(WTlifetime == 0)
        {
            if(getWorld().typeOfActor(getX(), getY()) == IID_DESTROYABLE_BRICK)
                changeLiveStatus();                 // Player dies if on a brick when effect ends
            canWalkThru = false;                    // Effect is no longer true
        }
    }
    int keyPress;
    if(getWorld().getKey(keyPress))
    {
        /*This switch-case checks to see if the position that the Player wants to go to is eligible
         *This switch-case also checks to see if the Player is allowed to drop a BugSprayer
         */
        switch(keyPress)
        {
            case(KEY_PRESS_LEFT):
                if(canMoveTo(getX()-1, getY())) moveTo(getX()-1, getY());
                break;
            case(KEY_PRESS_RIGHT):
                if(canMoveTo(getX()+1, getY())) moveTo(getX()+1, getY());
                break;
            case(KEY_PRESS_UP):
                if(canMoveTo(getX(), getY()+1)) moveTo(getX(), getY()+1);
                break;
            case(KEY_PRESS_DOWN):
                if(canMoveTo(getX(), getY()-1)) moveTo(getX(), getY()-1);
                break;
                /*The Player is not allowed to drop a BugSprayer on a brick or another BugSprayer
                 *The Player is also not allowed to drop more than the max number of BugSprayers
                 *Creates the new BugSprayer object and adds it to the World's object container
                 */
            case(KEY_PRESS_SPACE):
                if(getWorld().typeOfActor(getX(), getY()) != IID_DESTROYABLE_BRICK &&
                   getWorld().typeOfActor(getX(), getY()) != IID_BUGSPRAYER &&
                   getWorld().numberOfActors(IID_BUGSPRAYER) < getWorld().getMaxSprayers())
                {
                    Actor* Sprayer = new BugSprayer::BugSprayer(getX(), getY(), getWorld());
                    getWorld().addToContainer(Sprayer);
                }
                break;
        }
    }
}

void Player::addExtraLife()
{
    getWorld().incLives();                          // Add a life
}

void Player::walkThruWalls(int n)
{
    if(!canWalkThru) canWalkThru = true;            // Activates the walk through walls
    WTlifetime = n;
}

void Player::increaseSprayers(int n)
{
    if(!haveIncSprayers) haveIncSprayers = true;    // Activates the increased BugSprayers
    ISlifetime = n;
}

bool Player::canMoveTo(int x, int y)
{
    if(canWalkThru)                                 // Can walk through Destroyable Bricks
    {
        if(getWorld().typeOfActor(x, y) == IID_PERMA_BRICK) return false;
        return true;
    }
    else                                            // Cannot walk through Destroyable Bricks
    {
        if(getWorld().typeOfActor(x, y) == IID_PERMA_BRICK ||
           getWorld().typeOfActor(x, y) == IID_DESTROYABLE_BRICK) return false;
        return true;
    }
}

bool Zumi::canMove(int& n)
{
    if(n == 0)
    {
        n = maxNumTicks;
        return true;
    }
    n--;
    return false;
}

bool Zumi::canMoveTo(int x, int y)                  // Same as Player but also restricts BugSprayer
{
    if(getWorld().typeOfActor(x, y) == IID_PERMA_BRICK ||
       getWorld().typeOfActor(x, y) == IID_DESTROYABLE_BRICK ||
       getWorld().typeOfActor(x, y) == IID_BUGSPRAYER) return false;
    return true;
}

void Zumi::checkPlayerPos(Player* p)
{
    if(p->getX() == getX() &&                       // Same location as the Player before moving
       p->getY() == getY())
    {
        p->changeLiveStatus();
    }
}

void Zumi::deadBehavior()                           // Zumis have the chance of dropping Goodies
{
    getWorld().playSound(SOUND_ENEMY_DIE);
    /*If the Zumi does drop a Goodie, it randomly chooses to drop one based on the odds
     *If the Zumi does not drop either of the first two Goodies, it must drop the last Goodie
     */
    if(rand()%100 < getWorld().getGoodieProb())
    {
        if(rand()%100 < getWorld().getELGoodieProb())
        {
            ExtraLifeGoodie* el = new ExtraLifeGoodie::ExtraLifeGoodie(getX(), getY(), getWorld(), getWorld().getGoodieLife());
            getWorld().addToContainer(el);
            return;
        }
        else if(rand()%100 < getWorld().getWTGoodieProb())
        {
            WalkThruGoodie* wt = new WalkThruGoodie::WalkThruGoodie(getX(), getY(), getWorld(), getWorld().getGoodieLife(), getWorld().getWalkThruLife());
            getWorld().addToContainer(wt);
            return;
        }
        else                                        // Must drop this if it reaches here
        {
            IncSprayerGoodie* is = new IncSprayerGoodie::IncSprayerGoodie(getX(), getY(), getWorld(), getWorld().getGoodieLife(), getWorld().getBoostedSprayLife());
            getWorld().addToContainer(is);
            return;
        }
    }
    return;
}

void Zumi::simpleMove()
{
    /*If the Zumi cannot move to the position in the same direction it is currently going, change direction
     *Otherwise keep moving in the same direction
     */
    switch(currentDir)
    {
        case(SIMPLE_MOVE_LEFT):
            if(!canMoveTo(getX()-1, getY())) changeDir();
            else moveTo(getX()-1, getY());
            break;
        case(SIMPLE_MOVE_RIGHT):
            if(!canMoveTo(getX()+1, getY())) changeDir();
            else moveTo(getX()+1, getY());
            break;
        case(SIMPLE_MOVE_UP):
            if(!canMoveTo(getX(), getY()+1)) changeDir();
            else moveTo(getX(), getY()+1);
            break;
        case(SIMPLE_MOVE_DOWN):
            if(!canMoveTo(getX(), getY()-1)) changeDir();
            else moveTo(getX(), getY()-1);
            break;
    }

}

void SimpleZumi::doSomething()
{
    if(!isAlive()) return;
    Player* p = getWorld().getPlayer();
    checkPlayerPos(p);
    if(!p->isAlive()) return;
    if(!canMove(numTicks)) return;
    simpleMove();                                   // Simple movement
    checkPlayerPos(p);
    return;
}

void ComplexZumi::doSomething()
{
    if(!isAlive()) return;
    Player* p = getWorld().getPlayer();
    checkPlayerPos(p);
    if(!p->isAlive()) return;
    if(!canMove(numTicks)) return;
    if(horizDistFromPlayer() <= getWorld().getCZumiSmellDist() &&
       vertDistFromPlayer() <= getWorld().getCZumiSmellDist())
    {
        complexMove();                              // Complex movement if Player is "smellable"
        checkPlayerPos(p);
    }
    else simpleMove();                              // Otherwise simple movement
    checkPlayerPos(p);
    return;
}

int ComplexZumi::horizDistFromPlayer()              // Self-explanatory
{
    Player* p = getWorld().getPlayer();
    return abs(p->getX() - getX());
}

int ComplexZumi::vertDistFromPlayer()               // Self-explanatory
{
    Player* p = getWorld().getPlayer();
    return abs(p->getY() - getY());
}

/*This function is by far the most complicated one
 *This function uses breadth-first search to find a path to the Player, while maintaining the paths using a Node tree
 *Once the Player is found the tree is used to track the shortest path to the Player, and the Zumi moves one step in that direction
 *This function is called in doSomething(), meaning that it happens every tick
 */
void ComplexZumi::complexMove()
{
    queue<Node*> nodeQueue;                         // Create the queue of Nodes
    rootptr = new Node(getX(), getY());             // Create the root Node at the current Zumi location
    rootptr->parent = nullptr;                      // Initializations
    bool pathFound = false;
    nodeQueue.push(rootptr);
    vector<Coord> beenToCoord;                      // Create the vector of Coords that have been checked
    
    while(!nodeQueue.empty())
    {
        Node* curptr = nodeQueue.front();
        nodeQueue.pop();
        for(int i = 0; i < 4; i++)
        {
            curptr->child[i] = nullptr;             // Set all children to nullptr
        }
        
        if(curptr->m_x == getWorld().getPlayer()->getX() &&     // Exit condition
           curptr->m_y == getWorld().getPlayer()->getY())
        {
            pathFound = true;
            while(curptr->parent != rootptr)        // As long as the current pointer parent is not the root
            {
                curptr = curptr->parent;            // Move up the tree until you reach the first child
            }
            moveTo(curptr->m_x, curptr->m_y);       // Move the Zumi to this location
            break;
        }
        
        Node* lptr = new Node(curptr->m_x-1, curptr->m_y);      // Create Node pointers for each direction
        Node* rptr = new Node(curptr->m_x+1, curptr->m_y);
        Node* uptr = new Node(curptr->m_x, curptr->m_y+1);
        Node* dptr = new Node(curptr->m_x, curptr->m_y-1);
        
        /*If the Zumi can move to a neighboring position that has not been checked
         *This is done for each of the directions
         */
        if(canMoveTo(lptr->m_x, lptr->m_y) &&
           !coordChecked(lptr->m_x, lptr->m_y, beenToCoord))
        {
            Coord l(lptr->m_x, lptr->m_y);          // Creates a coordinate for the location
            beenToCoord.push_back(l);               // Sets coordinate to checked
            curptr->child[0] = lptr;                // Sets the first child of the parent
            lptr->parent = curptr;                  // Sets the first child's parent
            nodeQueue.push(lptr);                   // Push into the queue for further examination
        }
        if(canMoveTo(rptr->m_x, rptr->m_y) &&
           !coordChecked(rptr->m_x, rptr->m_y, beenToCoord))
        {
            Coord r(rptr->m_x, rptr->m_y);
            beenToCoord.push_back(r);
            curptr->child[1] = rptr;
            rptr->parent = curptr;
            nodeQueue.push(rptr);
        }
        if(canMoveTo(uptr->m_x, uptr->m_y) &&
           !coordChecked(uptr->m_x, uptr->m_y, beenToCoord))
        {
            Coord u(uptr->m_x, uptr->m_y);
            beenToCoord.push_back(u);
            curptr->child[2] = uptr;
            uptr->parent = curptr;
            nodeQueue.push(uptr);
        }
        if(canMoveTo(dptr->m_x, dptr->m_y) &&
           !coordChecked(dptr->m_x, dptr->m_y, beenToCoord))
        {
            Coord d(dptr->m_x, dptr->m_y);
            beenToCoord.push_back(d);
            curptr->child[3] = dptr;
            dptr->parent = curptr;
            nodeQueue.push(dptr);
        }
    }
    if(!pathFound) simpleMove();
}

bool ComplexZumi::coordChecked(int x, int y, vector<Coord> Coords)
{
    for(int i = 0; i < Coords.size(); i++)
    {
        if(x == Coords[i].x() &&
           y == Coords[i].y()) return true;
    }
    return false;
}

void BugSprayer::doSomething()
{
    if(!isAlive()) return;
    lifetime--;
    if(lifetime <= 0)                               // BugSprayer explodes
    {
        BugSpray* Spray = new BugSpray::BugSpray(getX(), getY(), getWorld());
        getWorld().addToContainer(Spray);           // Creates a BugSpray object where it exploded
        
        /*The following checks to see if the BugSpray can spread out
         *SpraySpread changes the value of "cont" in its function
         */
        bool cont = true, contl = true, contr = true, contu = true, contd = true;
        Spray->SpraySpread(getX(), getY(), getWorld(), cont);
        for(int i = 1; i <= BUGSPRAY_SPREAD; i++)
        {
            if(contl) Spray->SpraySpread(getX()-i, getY(), getWorld(), contl);
            if(contr) Spray->SpraySpread(getX()+i, getY(), getWorld(), contr);
            if(contu) Spray->SpraySpread(getX(), getY()+i, getWorld(), contu);
            if(contd) Spray->SpraySpread(getX(), getY()-i, getWorld(), contd);
        }
        getWorld().playSound(SOUND_SPRAY);
        changeLiveStatus();
    }
    return;
}

void BugSpray::doSomething()
{
    if(!isAlive()) return;
    Player* p = getWorld().getPlayer();
    if(p->getX() == getX() &&                       // Same location as Player
       p->getY() == getY())
    {
        p->changeLiveStatus();                      // Player dies
    }
    vector<Actor*> a = getWorld().getActors(getX(), getY());
    for(int i = 0; i < a.size(); i++)
    {
        Zumi* z = dynamic_cast<Zumi*>(a[i]);           // Same location as Zumi
        {
            if(z != nullptr)
            {
                z->changeLiveStatus();              // Zumi Dies
                if(z->getID() == IID_SIMPLE_ZUMI) getWorld().increaseScore(SIMPLE_ZUMI_SCORE);
                if(z->getID() == IID_COMPLEX_ZUMI) getWorld().increaseScore(COMPLEX_ZUMI_SCORE);
            }
        }
    }
    lifetime--;
    if(lifetime <= 0) changeLiveStatus();
    return;
}

/*This function checks to see what type of Actor is at the current location
 *Using dynamic casting, it can see what Actor it is and the corresponding action to take
 */
void BugSpray::SpraySpread(int x, int y, StudentWorld& World, bool& cont)
{
    cont = true;
    vector<Actor*> a = World.getActors(x, y);
    for(int i = 0; i < a.size(); i++)
    {
        DestroyableBrick* db = dynamic_cast<DestroyableBrick*>(a[i]);
        {
            if(db != nullptr)
            {
                Actor* Spray = new BugSpray::BugSpray(x, y, World);
                getWorld().addToContainer(Spray);
                db->changeLiveStatus();             // Brick is destroyed
                cont = false;                       // Cannot move past the brick
                return;
            }
        }
        PermanentBrick* pb = dynamic_cast<PermanentBrick*>(a[i]);
        {
            if(pb != nullptr)
            {
                cont = false;                       // Cannot move onto or past the brick
                return;
            }
        }
        BugSprayer* b = dynamic_cast<BugSprayer*>(a[i]);
        {
            if(b != nullptr)
            {
                b->setLifetimeToZero();             // Immediately triggers the BugSprayer
                return;
            }
        }
    }
    Actor* Spray = new BugSpray::BugSpray(x, y, World);
    getWorld().addToContainer(Spray);
}

void Goodies::decLifetime()
{
    lifetime--;
    if(lifetime <= 0) changeLiveStatus();
    return;
}

void Goodies::checkPlayer(int x, int y, StudentWorld& World)
{
    Player* p = World.getPlayer();
    if(p->getX() == x &&                            // Same location as Player
       p->getY() == y)
    {
        activated = true;                           // Activate the Goodie
        getWorld().increaseScore(GOODIE_SCORE);     // Increase the score
        getWorld().playSound(SOUND_GOT_GOODIE);     // Play the sound
        changeLiveStatus();                         // Delete the Goodie object
    }
}

/*Each of the Goodie doSomething functions check to see if the Goodie is alive, decrement the lifetime, and check to see if the Player is occupying the same coordinate
 *If so, then the Goodie is activated
 *Each function notifies the Player and the Player handles the effects of each Goodie
 */
void ExtraLifeGoodie::doSomething()
{
    if(!isAlive()) return;
    decLifetime();
    checkPlayer(getX(), getY(), getWorld());
    if(isActivated()) getWorld().getPlayer()->addExtraLife();
}

void WalkThruGoodie::doSomething()
{
    if(!isAlive()) return;
    decLifetime();
    checkPlayer(getX(), getY(), getWorld());
    if(isActivated()) getWorld().getPlayer()->walkThruWalls(lifetime);
}

void IncSprayerGoodie::doSomething()
{
    if(!isAlive()) return;
    decLifetime();
    checkPlayer(getX(), getY(), getWorld());
    if(isActivated()) getWorld().getPlayer()->increaseSprayers(lifetime);
}

void Exit::doSomething()
{
    if(getWorld().getPlayer()->getX() == getX() &&  // Player is at the correct coordinate
       getWorld().getPlayer()->getY() == getY() &&
       getWorld().isExitFound())                    // Exit has been revealed
    {
        getWorld().complete();                      // Level is complete
    }
}