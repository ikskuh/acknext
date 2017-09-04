#include "buffer.hpp"

Buffer::Buffer(GLenum type) :
    EngineObject<BUFFER>()
{
	glCreateBuffers(1, &this->api().object);
	api().type = type;
	api().size = 0;
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &this->api().object);
}

ACKNEXT_API_BLOCK
{
	BUFFER * buffer_create(GLenum type)
	{
		return demote(new Buffer(type));
	}

	void buffer_set(BUFFER * buffer, size_t size, void * data)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return;
		}
		glNamedBufferData(
			buffer->object,
			size,
		    data,
			GL_DYNAMIC_DRAW); // TODO: Fix this later
		buffer->size = size;
	}

	void buffer_update(BUFFER * buffer, size_t offset, size_t size, void * data)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return;
		}
		if(data == NULL) {
			engine_seterror(ERR_INVALIDARGUMENT, "data must be set.");
			return;
		}
		if((offset + size) > size_t(buffer->size)) {
			engine_seterror(ERR_INVALIDARGUMENT, "offset and size must contained in the buffer.");
			return;
		}
		glNamedBufferSubData(
			buffer->object,
			offset,
			size,
		    data);
	}

	ACKFUN void * buffer_map(BUFFER * buffer, GLenum mode)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return nullptr;
		}
		switch(mode) {
			case READONLY:
			case WRITEONLY:
			case READWRITE:
				break;
			default:
				engine_seterror(ERR_INVALIDARGUMENT, "Invalid access mode!");
				return nullptr;
		}
		return glMapNamedBuffer(buffer->object, mode);
	}

	ACKFUN void buffer_unmap(BUFFER * buffer)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return;
		}
		glUnmapNamedBuffer(buffer->object);
	}

	void buffer_remove(BUFFER * buffer)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf) {
			delete buf;
		}
	}
}
