#include <iostream>
#include <list>
#include <memory>
#include <algorithm>

#define TRACE(...) {std::cout << "Line: " << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

class Rock
{
public:
    Rock(double ix, double iy) : x(ix), y(iy) 
    {
        TRACE(__FUNCTION__ << ", x:" << x << ", y: " << y);
    }
    
    ~Rock()
    {
        TRACE(__FUNCTION__ << ", x:" << x << ", y: " << y);
    }

    double x;
    double y;
};

class RockField
{
public:
    typedef std::unique_ptr<Rock> RockPtr_t;
    typedef std::list<RockPtr_t> RocksList_t;

    RockField() { 
        TRACE(__FUNCTION__); 
    }

    ~RockField() {
        TRACE(__FUNCTION__);
    }

    void CreateOne(double x, double y) {
        RockPtr_t rock(new Rock(x, y));
        m_rocks.push_back(std::move(rock));
    }

    void DeleteOne(RockField::RocksList_t::iterator rock)
    {
        TRACE(__FUNCTION__);

        m_rocks.erase(rock);
    }

    RocksList_t m_rocks;
};

int main()
{
    RockField rf;

    rf.CreateOne(0, 0);
    rf.CreateOne(1, 1);
    rf.CreateOne(2, 2);

    for (auto& rock : rf.m_rocks)
        TRACE(__FUNCTION__ << "Rock: x: " << rock->x << ", y: " << rock->y);

    // Make list of RocksList_t iterators, each stores a rock to be deleted.
    // (Deleting from a list while iterating through it causes mayhem)
    std::list<RockField::RocksList_t::iterator> dt;
    RockField::RocksList_t::iterator it;

    for (it = rf.m_rocks.begin(); it != rf.m_rocks.end(); it++)
    {
        auto rock = it->get();

        TRACE(__FUNCTION__ << "Rock: x: " << rock->x << ", y: " << rock->y);
        if (rock->x == 1)
            dt.push_back(it);
    }

    for (auto it : dt)
    {
        rf.DeleteOne(it);
    }

    TRACE("End of main()");
}
