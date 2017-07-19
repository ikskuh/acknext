#ifndef ENTITYDETAIL_H
#define ENTITYDETAIL_H

#include "engine.h"
#include "model.h"

struct ENTITYdetail
{
	ENTITY * ent;
	Model * model;

	dBody * body;
	dGeom * shapeGeom, * polygonGeom;

	ENTITYdetail(ENTITY * ent);
	ENTITYdetail(ENTITYdetail const &) = delete;
	ENTITYdetail(ENTITYdetail &&) = delete;
	~ENTITYdetail();
};

#endif // ENTITYDETAIL_H
