#ifndef HULL_HPP
#define HULL_HPP

#include <engine.hpp>

#include <ode/ode.h>

class Hull :
	public EngineObject<HULL>
{
public:
	static Hull * fromGeom(dGeomID geom);
public:
	ENTITY * owner;
	dGeomID geom;
public:
	Hull(ENTITY * owner, dGeomID geom);
	NOCOPY(Hull);
	~Hull();

	void update();
};

#endif // HULL_HPP
