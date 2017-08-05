#ifndef MODEL_HPP
#define MODEL_HPP

#include <engine.hpp>

class Model : public EngineObject<MODEL>
{
public:
	Model();
	NOCOPY(Model);
	~Model();
};

#endif // MODEL_HPP
