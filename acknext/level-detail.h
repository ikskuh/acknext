#ifndef LEVELDETAIL_H
#define LEVELDETAIL_H

#include <acknext.h>
#include <set>

struct LEVELdetail
{
    LEVEL * const level;

    std::set<ENTITY*> entities;

    LEVELdetail(LEVEL *level) :
        level(level),
        entities()
    {

    }

    ~LEVELdetail()
    {

    }
};

#endif // LEVELDETAIL_H
