#include "collisionsystem.hpp"

#include "hull.hpp"

#include <vector>
#include <algorithm>

dSpaceID CollisionSystem::space;

void CollisionSystem::initialize()
{
	dInitODE();

	space = dHashSpaceCreate(0);
}


void CollisionSystem::update()
{

}

void CollisionSystem::shutdown()
{
	dSpaceDestroy(space);
	dCloseODE();
}

static COLLISION * allocTempCollision()
{
	static COLLISION buffer[ACKNEXT_TEMP_COLLISIONS];
	static int index = 0;
	return reinterpret_cast<COLLISION*>(memset(
		&buffer[index++ % ACKNEXT_TEMP_COLLISIONS],
		0,
		sizeof(COLLISION)));
}

struct TraceFeedback
{
	std::vector<COLLISION> collisions;
};

static void dTraceCallback (void *data, dGeomID o1, dGeomID o2)
{
	if (dGeomIsSpace (o1) || dGeomIsSpace (o2)) {
		engine_log("Subspace collision detected! Not supported yet!");
		return;
	}

	TraceFeedback * feedback = reinterpret_cast<TraceFeedback*>(data);

	// colliding two non-space geoms, so generate contact
	// points between o1 and o2

	dContactGeom contacts[ACKNEXT_MAX_CONTACTS];

	int num_contact = dCollide(
		o1,
		o2,
		(0xFFFF & ACKNEXT_MAX_CONTACTS),
		contacts,
		sizeof(dContactGeom));
	if(num_contact <= 0) {
		return;
	}

	for(int i = 0; i < num_contact; i++)
	{
		dContactGeom const & c = contacts[i];

		COLLISION col;
		col.contact.x = c.pos[0];
		col.contact.y = c.pos[1];
		col.contact.z = c.pos[2];

		col.normal.x = c.normal[0];
		col.normal.y = c.normal[1];
		col.normal.z = c.normal[2];

		if(dGeomGetClass(c.g1) == dRayClass) {
			col.hull = demote(Hull::fromGeom(c.g2));
		} else {
			col.hull = demote(Hull::fromGeom(c.g1));
		}

		feedback->collisions.emplace_back(col);
	}
}

ACKNEXT_API_BLOCK
{
	ACKFUN COLLISION * c_trace(VECTOR const * _from, VECTOR const * _to, BITFIELD mask)
	{
		VECTOR from = *_from;
		VECTOR to = *_to;
		VECTOR * dist = vec_diff(nullptr, &to, &from);

		dGeomID ray = dCreateRay(0, vec_length(dist));

		vec_normalize(dist, 1);
		dGeomRaySet(
			ray,
			from.x, from.y, from.z,
			dist->x, dist->y, dist->z);

		TraceFeedback feedback;
		dSpaceCollide2(
			reinterpret_cast<dGeomID>(CollisionSystem::space),
			ray,
			&feedback,
			dTraceCallback);

		if(feedback.collisions.size() == 0) {
			return nullptr;
		}

		for(COLLISION &c : feedback.collisions) {
			c.distance = vec_dist(&from, &c.contact);
		}

		std::sort(
			feedback.collisions.begin(),
			feedback.collisions.end(),
			[](COLLISION & l, COLLISION & r) {
				return l.distance < r.distance;
			});

		// TODO: Store multiple trace results in
		// user-accessible array

		dGeomDestroy(ray);

		COLLISION *tmp = allocTempCollision();
		*tmp = feedback.collisions[0];
		return tmp;
	}

}
