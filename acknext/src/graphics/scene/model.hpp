#ifndef MODEL_HPP
#define MODEL_HPP

#include <engine.hpp>

#include <vector>

class Model : public EngineObject<MODEL>
{
public:
	bool userCreated;
public:
	explicit Model();
	NOCOPY(Model);
	~Model();
};

#endif // MODEL_HPP
