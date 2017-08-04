#ifndef BUFFERDETAIL_H
#define BUFFERDETAIL_H

#include "engine.h"

struct BUFFER
{
	GLuint id;
	GLenum type;
	GLsizei size;

	explicit BUFFER(GLenum type);

	BUFFER(BUFFER const &) = delete;
	BUFFER(BUFFER &&) = delete;

	~BUFFER();
};

#endif // BUFFERDETAIL_H
