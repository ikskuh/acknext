#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <engine.hpp>

class Material :
	public EngineObject<MATERIAL>
{
public:
	bool userCreated;
public:
	explicit Material(bool userCreated);
	NOCOPY(Material);
	~Material();
};

#endif // MATERIAL_HPP
