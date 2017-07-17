#include "engine.h"
#include "buffer-detail.h"

BUFFER::BUFFER(GLenum type) : id(0), type(type), size(0)
{
	glCreateBuffers(1, &this->id);
}

BUFFER::~BUFFER()
{
	glDeleteBuffers(1, &this->id);
}

GLenum translate(enum BUFFERTYPE type)
{
	switch(type)
	{
		case VERTEX_BUFFER: return GL_ARRAY_BUFFER;
		case INDEX_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
		case UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
		default:
			engine_seterror(INVALID_ARGUMENT, "%d is not a valid buffer type!", type);
			return GL_INVALID_ENUM;
	}
}

ACKFUN BUFFER * buffer_create(enum BUFFERTYPE type)
{
	GLenum gltype = translate(type);
	if(gltype == GL_INVALID_ENUM) {
		return nullptr;
	}
	return new BUFFER(gltype);
}

ACKFUN void buffer_set(BUFFER * buffer, size_t size, void * data)
{
	if(buffer == NULL) {
		engine_seterror(INVALID_ARGUMENT, "buffer must be set.");
		return;
	}
	if(data == NULL) {
		engine_seterror(INVALID_ARGUMENT, "data must be set.");
		return;
	}
	glNamedBufferData(
		buffer->id,
		size,
	    data,
		GL_STATIC_DRAW);
	buffer->size = size;
}

ACKFUN void buffer_update(BUFFER * buffer, size_t offset, size_t size, void * data)
{
	if(buffer == NULL) {
		engine_seterror(INVALID_ARGUMENT, "buffer must be set.");
		return;
	}
	if(data == NULL) {
		engine_seterror(INVALID_ARGUMENT, "data must be set.");
		return;
	}
	if((offset + size) > size_t(buffer->size)) {
		engine_seterror(INVALID_ARGUMENT, "offset and size must contained in the buffer.");
		return;
	}
	glNamedBufferSubData(
		buffer->id,
		offset,
		size,
	    data);
}

ACKFUN unsigned int buffer_getObject(BUFFER * buffer)
{
	if(buffer == NULL) {
		engine_seterror(INVALID_ARGUMENT, "buffer must be set.");
		return 0;
	}
	return buffer->id;
}

ACKFUN void buffer_remove(BUFFER * buffer)
{
	if(buffer == nullptr) {
		engine_seterror(INVALID_ARGUMENT, "buffer must not be NULL!");
		return;
	}
	delete buffer;
}
