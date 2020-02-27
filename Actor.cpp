#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(int imID, double startX, double startY, StudentWorld* swptr, Direction dir, int depth) :GraphObject(imID, startX, startY, dir, depth)
{
    m_alive = true;
    sw = swptr;
}

StudentWorld* Actor::getWorld() const
{
    return sw;
}

bool Actor::isAlive() { return m_alive; }
void Actor::setDead() { m_alive = false; }


bool Actor::takeDamage(int damage) { return false; }
bool Actor::blocksBacteria() const { return false; }
bool Actor::isEdible() const { return false; }

void Actor::doSomething() {}



Dirt::Dirt(double startX, double startY, StudentWorld* swptr) :Actor(IID_DIRT, startX, startY, swptr, 0, 1)
{}

Food::Food(double startX, double startY, StudentWorld* swptr) : Actor(IID_FOOD, startX, startY, swptr, 0, 1)
{}

Pit::Pit(double startX, double startY, StudentWorld* swptr) : Actor(IID_PIT, startX, startY, swptr, 0, 1)
{
    m_regSalmonella = 5;
    m_aggSalmonella = 3;
    m_eColi = 2;
}
void Pit::doSomething()
{

    if (randInt(1, 50) == 1)
    {
        int b = randInt(1, 3);
        switch (b)
        {
        case 1:
            if (m_regSalmonella > 0)
            {
                m_regSalmonella--;
                getWorld()->addActor(new RegularSalmonella(getX(), getY(), getWorld()));
                getWorld()->playSound(SOUND_BACTERIUM_BORN);
                break;
            }
        case 2:
            if (m_aggSalmonella > 0)
            {
                m_aggSalmonella--;
                getWorld()->addActor(new AggressiveSalmonella(getX(), getY(), getWorld()));
                getWorld()->playSound(SOUND_BACTERIUM_BORN);
                break;
            }
        case 3:
            if (m_eColi > 0)
            {
                m_eColi--;
                getWorld()->addActor(new eColi(getX(), getY(), getWorld()));
                getWorld()->playSound(SOUND_BACTERIUM_BORN);
                break;
            }
            
        };
    }
}



Projectile::Projectile(const int id, double startX, double startY, StudentWorld* swptr, Direction d, int maxLife) :Actor(id, startX, startY, swptr, d, 1)
{
    m_maxLife = maxLife;
}

void Projectile::doSomething()
{
    moveForward(SPRITE_RADIUS * 2);
    m_maxLife -= (SPRITE_RADIUS * 2);
}

Spray::Spray(double startX, double startY, StudentWorld* swptr, Direction d) : Projectile(IID_SPRAY, startX, startY, swptr, d, 112)
{
}

void Spray::doSomething()
{
    Projectile::doSomething();
    if (getWorld()->overlapsProjectile(getX(), getY(), 2))
    {
        setDead();
        return;
    }
    if (isMax())
    {
        setDead();
    }
}

Flame::Flame(double startX, double startY, StudentWorld* swptr, Direction d) : Projectile(IID_FLAME, startX, startY, swptr, d, 32)
{
}
void Flame::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    if (isAlive())
    {
        Projectile::doSomething();
        if (getWorld()->overlapsProjectile(getX(), getY(),5))
        {
            setDead();
            return;
        }
        if(isMax())
        {
            setDead();
        }
    }
}


Goodie::Goodie(const int id, double x, double y, StudentWorld* swptr) :Actor(id, x, y, swptr, 0, 1)
{
    m_maxLife = max(randInt(0, (300 - 10 * getWorld()->getLevel())), 50);
}
void Goodie::doSomething()
{

    m_maxLife--;
    if (m_maxLife == 0)
    {
        setDead();
    }
}
//bool Goodie::takeDamage(int) { setDead(); return true; }

RestoreHealthGoodie::RestoreHealthGoodie(double x, double y, StudentWorld* swptr) : Goodie(IID_RESTORE_HEALTH_GOODIE, x, y, swptr)
{}
void RestoreHealthGoodie::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    else if (isAlive())
    {
        Socrates* s = getWorld()->getOverlappingSocrates(this);
        if (s != nullptr)
        {
            getWorld()->increaseScore(250);
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            s->restoreHealth();
            return;
        }
    }
}
FlameThrowerGoodie::FlameThrowerGoodie(double x, double y, StudentWorld* swptr) : Goodie(IID_FLAME_THROWER_GOODIE, x, y, swptr)
{}
void FlameThrowerGoodie::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    else if (isAlive())
    {
        Socrates* s = getWorld()->getOverlappingSocrates(this);
        if (s != nullptr)
        {
            getWorld()->increaseScore(300);
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            s->restoreFlame();
            return;
        }
    }
}

ExtraLifeGoodie::ExtraLifeGoodie(double x, double y, StudentWorld* swptr) : Goodie(IID_FLAME_THROWER_GOODIE, x, y, swptr)
{}
void ExtraLifeGoodie::doSomething()
{

    if (!isAlive())
    {
        return;
    }
    else if (isAlive())
    {
        Socrates* s = getWorld()->getOverlappingSocrates(this);
        if (s != nullptr)
        {
            getWorld()->increaseScore(300);
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            s->restoreFlame();
            return;
        }
    }
}

Fungus::Fungus(double x, double y, StudentWorld* swptr) : Goodie(IID_FUNGUS, x, y, swptr)
{}
void Fungus::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    else if (isAlive())
    {
        Socrates* s = getWorld()->getOverlappingSocrates(this);
        s->setDead();
        if (s != nullptr)
        {
            getWorld()->increaseScore(-50);
            setDead();
            s->takeDamage(20);
            return;
        }
    }
}


Agent::Agent(const int id, double startX, double startY, StudentWorld* swptr, int hp, Direction dir, int depth, double size): Actor(id, startX, startY, swptr)
{
    m_hp = hp;
}
bool Agent::takeDamage(int damage) 
{ 
    m_hp -= damage; 
    if(m_hp<=0)
    {
        setDead();
    }
    return true;
}
int Agent::getHitPoints() const { return m_hp; }




Socrates::Socrates(double startX, double startY, StudentWorld* swptr) :Agent(IID_PLAYER, startX, startY, swptr, 100)
{
    m_sprayCharges=20;
    m_flameCharges=5;
}

void Socrates::moveSocrates(double d)
{
    double x, y;
    Direction a = getDirection() + d;
    getPositionInThisDirection(a, VIEW_RADIUS, x, y);
    moveTo(VIEW_WIDTH / 2 - x + getX(), VIEW_HEIGHT / 2 - y + getY());
    setDirection(a);
}

void Socrates::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    else
    {
        int ch;
        if (Actor::getWorld()->getKey(ch))
        {
            // user hit a key during this tick!
            switch (ch)
            {
            case KEY_PRESS_LEFT:
                //move Socrates counterclockwise
            {
                moveSocrates(-5.0);
                break;
            }
            case KEY_PRESS_RIGHT:
                //... move Socrates clockwise...;  
            {
                moveSocrates(+5.0);
                break;

            }
            case KEY_PRESS_SPACE:
                //... add spray in front of Socrates...; 
            {
                if (m_sprayCharges >= 1)
                {
                    double sprayX, sprayY;
                    getPositionInThisDirection(getDirection(), 2 * SPRITE_RADIUS, sprayX, sprayY);
                    getWorld()->addActor(new Spray(sprayX, sprayY, getWorld(), getDirection()));
                    m_sprayCharges--;
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                };
                break;
            }
            case KEY_PRESS_ENTER:
            {
                if (m_flameCharges > 0)
                {
                    Direction right = getDirection() - 22;
                    Direction left = getDirection();
                    for (int i = 0; i < 8; i++)
                    {
                        double flameX = 0.0;
                        double flameY = 0.0;
                        getPositionInThisDirection(right, 2 * SPRITE_RADIUS, flameX, flameY);
                        getWorld()->addActor(new Flame(flameX, flameY, getWorld(), right));
                        getPositionInThisDirection(left, 2 * SPRITE_RADIUS, flameX, flameY);
                        getWorld()->addActor(new Flame(flameX, flameY, getWorld(), left));
                        right -= 22;
                        left += 22;
                    }
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_flameCharges--;
                }
                break;
            }
            }
        }
    }
}

Bacteria::Bacteria(const int id, double startX, double startY, StudentWorld* swptr, int hp):Agent(id, startX, startY, swptr, hp)
{}


Salmonella::Salmonella(const int id, double startX, double startY, StudentWorld* swptr, int hp) :Bacteria(id, startX, startY, swptr, hp)
{
    mpd = 50;
    m_food = 0;
}


RegularSalmonella::RegularSalmonella(double startX, double startY, StudentWorld* swptr) :Salmonella(IID_SALMONELLA, startX, startY, swptr, 4)
{}

void RegularSalmonella::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    /*else if (isAlive())
    {
        if (getWorld()->overlapsSocrates(getX(), getY(), 1))
        { }
        else if (getFood() == 3)
        {
            double newX, newY;
            if (getX() <= VIEW_WIDTH / 2)
            {
                newX = getX() + SPRITE_RADIUS;
            }
            else if (getX() > VIEW_WIDTH / 2)
            {
                newX = getX() - SPRITE_RADIUS;
            }

            if (getY() <= VIEW_WIDTH / 2)
            {
                newY = getY() + SPRITE_RADIUS;
            }
            else if (getY() > VIEW_WIDTH / 2)
            {
                newY = getY() - SPRITE_RADIUS;
            }

            getWorld()->addActor(new RegularSalmonella(newX, newY, getWorld()));
            resetFood();
        }
        if (getWorld()->getOverlappingEdible(this))
        {
            Actor* food = getWorld()->getOverlappingEdible(this);
        }
    }
    if (getMPD() > 0)
    {
        changeMPD(-1);
        double threeX, threeY;
        getPositionInThisDirection(getDirection(), 3, threeX, threeY);
        int r = getWorld()->findEuclidean(threeX, threeY, VIEW_WIDTH / 2, VIEW_HEIGHT / 2);
        if (r < VIEW_RADIUS && getWorld()->isBacteriumMovementBlockedAt(this,threeX, threeY))
        {
            moveTo(threeX, threeY);
        }
    }
    else
    {
        Direction rand = randInt(0, 359);
        setDirection(rand);
        resetMPD();
    }*/
}

AggressiveSalmonella::AggressiveSalmonella(double startX, double startY, StudentWorld* swptr) :Salmonella(IID_SALMONELLA, startX, startY, swptr, 4)
{}

eColi::eColi(double startX, double startY, StudentWorld* swptr) : Bacteria(IID_ECOLI, startX, startY, swptr, 4)
{}