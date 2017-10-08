#include <engine.hpp>

#include <physfs.h>
#include "core/config.hpp"

ACKNEXT_API_BLOCK
{
	void filesys_addResource(char const * resource, char const * path)
	{
		if(!(engine_config.flags & USE_VFS)) {
			engine_seterror(ERR_INVALIDOPERATION, "acknext was not configured to use the virtual file system.");
			return;
		}
		ARG_NOTNULL(resource, );
		if(PHYSFS_mount(resource, path, 1) == 0) {
			engine_seterror(ERR_FILESYSTEM, "%s", PHYSFS_getLastError());
		}
	}
}

static Sint64 rw_size(SDL_RWops * c)
{
	return file_size((ACKFILE*)c->hidden.unknown.data1);
}

static Sint64 rw_seek(SDL_RWops *c,Sint64 offset,int whence)
{
	auto * f = (ACKFILE*)c->hidden.unknown.data1;
	if(whence == RW_SEEK_CUR)
	{
		offset = file_tell(f) + offset;
	}
	else if(whence == RW_SEEK_END)
	{
		auto len = file_size(f);
		if(len < 0) return SDL_SetError("Can't seek with _END in a non-sized stream!");
		offset = len - offset;
	}
	file_seek(f, offset);
	return offset;
}

static size_t rw_read(SDL_RWops *c, void *ptr, size_t size, size_t num)
{
	return file_read((ACKFILE*)c->hidden.unknown.data1, ptr, size * num) / size;
}

static size_t rw_write(SDL_RWops *c, const void *ptr, size_t size, size_t num)
{
	return file_write((ACKFILE*)c->hidden.unknown.data1, ptr, size * num) / size;
}

static int rw_close(SDL_RWops *context)
{
	file_close((ACKFILE*)context->hidden.unknown.data1);
	return 0;
}

SDL_RWops * SDL_RWFromAcknext(ACKFILE * file)
{
	ARG_NOTNULL(file, nullptr);

	SDL_RWops *rwops = SDL_AllocRW();
	assert(rwops);

	rwops->type = 0xdeadbeef;
	rwops->hidden.unknown.data1 = file;

	rwops->size = rw_size;
	rwops->seek = rw_seek;
	rwops->read = rw_read;
	rwops->write = rw_write;
	rwops->close= rw_close;

	return rwops;
}
