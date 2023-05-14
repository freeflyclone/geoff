#include <iostream>
#include <list>
#include <memory>
#include <algorithm>

#define TRACE(...) {std::cout << "Line: " << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

class RockField;
class Rock
{
public:
    Rock(RockField& field, double ix, double iy) : m_rockField(field), x(ix), y(iy) 
    {
        TRACE(__FUNCTION__);
    }
    
    ~Rock()
    {
        TRACE(__FUNCTION__);
    }

    RockField& m_rockField;
    double x;
    double y;
};

struct RockDeleter
{
    void operator ()(Rock* rockPtr)
    {
        TRACE(__FUNCTION__);
        delete rockPtr;
    }
};

class RockField
{
public:
    typedef std::unique_ptr<Rock, RockDeleter> RockPtr_t;
    typedef std::list<RockPtr_t> RocksList_t;


    RockField() { 
        TRACE(__FUNCTION__); 
    }

    ~RockField() {
        TRACE(__FUNCTION__);
    }

    void CreateOne(double x, double y) {
        RockPtr_t rock(new Rock(*this, x, y));
        m_rocks.push_back(std::move(rock));
    }

    RocksList_t m_rocks;
};

int main()
{
    RockField rf;

    rf.CreateOne(0, 0);

    TRACE("End of main()");
}
