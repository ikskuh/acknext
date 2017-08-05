#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <engine.hpp>

class Buffer : public EngineObject<BUFFER>
{
public:
	GLuint id;
	GLenum type;
public:
	explicit Buffer(BUFFERTYPE type);
	NOCOPY(Buffer);
	~Buffer();
};

#endif // BUFFER_HPP
