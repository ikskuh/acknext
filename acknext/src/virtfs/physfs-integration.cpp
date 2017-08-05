#include <engine.hpp>

#include <physfs.h>

ACKNEXT_API_BLOCK
{
	void filesys_addResource(char const * resource, char const * path)
	{
		if(resource == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "name must not be NULL");
			return;
		}
		if(PHYSFS_mount(resource, path, 1) == 0) {
			engine_seterror(ERR_FILESYSTEM, "%s", PHYSFS_getLastError());
		}
	}

	ACKFILE * file_open_read(char const * name)
	{
		if(name == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "name must not be NULL");
			return nullptr;
		} else {
			return PHYSFS_openRead(name);
		}
	}

	ACKFILE * file_open_write(char const * name)
	{
		if(name == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "name must not be NULL");
			return nullptr;
		} else {
			return PHYSFS_openWrite(name);
		}
	}

	ACKFILE * file_open_append(char const * name)
	{
		if(name == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "name must not be NULL");
			return nullptr;
		} else {
			return PHYSFS_openAppend(name);
		}
	}

	int64_t file_read(ACKFILE *file, void *buffer, uint32_t objSize, uint32_t objCount)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return -1;
		} else {
			return PHYSFS_read(file, buffer, objSize, objCount);
		}
	}

	int64_t file_write(ACKFILE *file, const void *buffer, uint32_t objSize, uint32_t objCount)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return -1;
		} else {
			return PHYSFS_write(file, buffer, objSize, objCount);
		}
	}

	void file_seek(ACKFILE * file, uint64_t position)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
		} else {
			PHYSFS_seek(file, position);
		}
	}

	bool file_eof(ACKFILE * file)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return true;
		} else {
			return PHYSFS_eof(file);
		}
	}

	int64_t file_tell(ACKFILE * file)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return true;
		} else {
			return PHYSFS_tell(file);
		}
	}

	int64_t file_size(ACKFILE * file)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return true;
		} else {
			return PHYSFS_fileLength(file);
		}
	}

	void file_close(ACKFILE * file)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
		} else {
			PHYSFS_close(file);
		}
	}
}
