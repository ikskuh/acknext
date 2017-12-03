#include "hull.hpp"

#include "../scene/entity.hpp"
#include "collisionsystem.hpp"

Hull::Hull(ENTITY * owner, dGeomID geom) :
    owner(owner),
    geom(geom)
{
	dGeomSetData(this->geom, this);

	api().entity = this->owner;
	api().type = dGeomGetClass(this->geom);

	this->update();
}

Hull::~Hull()
{
	dGeomSetData(this->geom, nullptr);
	dGeomDestroy(this->geom);
}

Hull * Hull::fromGeom(dGeomID geom)
{
	return reinterpret_cast<Hull*>(dGeomGetData(geom));
}

void Hull::update()
{
	dGeomSetCategoryBits(this->geom, api().entity->categories);

	if(dGeomGetClass(this->geom) == dHeightfieldClass)
		return;

	dQuaternion quat = {
	    this->owner->rotation.w,
		this->owner->rotation.x,
		this->owner->rotation.y,
		this->owner->rotation.z
	};

	dGeomSetPosition(
		this->geom,
		this->owner->position.x,
		this->owner->position.y,
		this->owner->position.z);
	dGeomSetQuaternion(
		this->geom,
		quat);
}

ACKNEXT_API_BLOCK
{
	HULL * hull_createBox(ENTITY * ent, VECTOR const * size)
	{
		if(ent == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "ent must not be NULL!");
			return nullptr;
		}
		if(size == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "size must not be NULL!");
			return nullptr;
		}
		return demote(new Hull(ent, dCreateBox(
			collision_space,
			size->x,
			size->y,
			size->z)));
	}

	HULL * hull_createSphere(ENTITY * ent, float radius)
	{
		if(ent == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "ent must not be NULL!");
			return nullptr;
		}
		if(radius <= 0.0) {
			engine_seterror(ERR_INVALIDARGUMENT, "radius must be positive!");
			return nullptr;
		}
		return demote(new Hull(ent, dCreateSphere(
			collision_space,
			radius)));
	}

	HULL * hull_createFromExisting(ENTITY * ent, dGeomID geom)
	{
		ARG_NOTNULL(ent,  nullptr);
		ARG_NOTNULL(geom, nullptr);
		if(dGeomGetSpace(geom) != collision_space) {
			engine_seterror(ERR_INVALIDARGUMENT, "geom must be in collision_space!");
			return nullptr;
		}
		return demote(new Hull(ent, geom));
	}

	void hull_remove(HULL * _hull)
	{
		Hull * hull = promote<Hull>(_hull);
		if(hull) {
			delete hull;
		}
	}
}
