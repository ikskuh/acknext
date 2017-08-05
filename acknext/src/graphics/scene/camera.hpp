#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <engine.hpp>

class Camera : public EngineObject<CAMERA>
{
public:
	bool userCreated;
public:
	explicit Camera();
	NOCOPY(Camera);
	~Camera();
};

#endif // CAMERA_HPP
