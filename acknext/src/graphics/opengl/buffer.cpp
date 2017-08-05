#include "buffer.hpp"

static GLenum getBufferType(BUFFERTYPE type)
{
	switch(type) {
		case VERTEXBUFFER: return GL_ARRAY_BUFFER;
		case INDEXBUFFER: return GL_ELEMENT_ARRAY_BUFFER;
		default:
			engine_seterror(ERR_INVALIDARGUMENT, "Invalid buffer type!");
			return GL_INVALID_ENUM;
	}
}

Buffer::Buffer(BUFFERTYPE type) :
    EngineObject<BUFFER>(),
	type(getBufferType(type))
{
	glCreateBuffers(1, &this->id);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &this->id);
}

ACKNEXT_API_BLOCK
{
	BUFFER * buffer_create(BUFFERTYPE type)
	{
		GLenum t = getBufferType(type);
		if(t == GL_INVALID_ENUM) {
			return nullptr;
		}
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
			buf->id,
			size,
		    data,
			GL_STATIC_DRAW);
		buf->size = size;
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
		if((offset + size) > size_t(buf->size)) {
			engine_seterror(ERR_INVALIDARGUMENT, "offset and size must contained in the buffer.");
			return;
		}
		glNamedBufferSubData(
			buf->id,
			offset,
			size,
		    data);
	}

	GLDATA buffer_getObject(BUFFER * buffer)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return 0;
		}
		return buf->id;
	}

	void buffer_remove(BUFFER * buffer)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf) {
			delete buf;
		}
	}
}
