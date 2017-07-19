#ifndef LEVELDETAIL_H
#define LEVELDETAIL_H

#include "engine.h"
#include <set>

struct LEVELdetail
{
    LEVEL * const level;

    std::set<ENTITY*> entities;

	dWorld world;
	dSimpleSpace space;
	dJointGroup contactGroup;

    LEVELdetail(LEVEL *level) :
        level(level),
        entities(),
	    world(),
	    space(),
	    contactGroup()
    {
		this->world.setGravity(0.0, -9.81, 0.0);
    }

    ~LEVELdetail()
    {

    }
};

#endif // LEVELDETAIL_H
