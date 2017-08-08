#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include <engine.hpp>

#include <ode/ode.h>
#include <ode/collision.h>

class CollisionSystem
{
public:
	static dSpaceID space;
public:
	CollisionSystem() = delete;

	static void initialize();

	static void update();

	static void shutdown();
};

#endif // COLLISIONSYSTEM_HPP
