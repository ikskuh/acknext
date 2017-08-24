#include "buffer.hpp"

static GLenum getBufferType(BUFFERTYPE type)
{
	switch(type) {
		case VERTEXBUFFER: return GL_ARRAY_BUFFER;
		case INDEXBUFFER: return GL_ELEMENT_ARRAY_BUFFER;
		case UNIFORMBUFFER: return GL_UNIFORM_BUFFER;
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
	api().type = type;
	api().size = 0;
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
			buf->id,
			offset,
			size,
		    data);
	}

	ACKFUN void * buffer_map(BUFFER * buffer, ACCESSMODE mode)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return nullptr;
		}
		GLenum glmode;
		switch(mode) {
			case READONLY: glmode = GL_READ_ONLY; break;
			case WRITEONLY: glmode = GL_READ_WRITE; break;
			case READWRITE: glmode = GL_WRITE_ONLY; break;
			default:
				engine_seterror(ERR_INVALIDARGUMENT, "Invalid access mode!");
				return nullptr;
		}
		return glMapNamedBuffer(buf->id, glmode);
	}

	ACKFUN void buffer_unmap(BUFFER * buffer)
	{
		Buffer * buf = promote<Buffer>(buffer);
		if(buf == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "buffer must not be null!");
			return;
		}
		glUnmapNamedBuffer(buf->id);
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
