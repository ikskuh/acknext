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

	ACKFILE * file_open_read(char const * fileName)
	{
		ARG_NOTNULL(fileName,nullptr);
		return PHYSFS_openRead(fileName);
	}

	ACKFILE * file_open_write(char const * fileName)
	{
		ARG_NOTNULL(fileName,nullptr);
		return PHYSFS_openWrite(fileName);
	}

	ACKFILE * file_open_append(char const * fileName)
	{
		ARG_NOTNULL(fileName,nullptr);
		return PHYSFS_openAppend(fileName);
	}

	int64_t file_read(ACKFILE *file, void *buffer, uint32_t size)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return -1;
		} else {
			return PHYSFS_readBytes(file, buffer, size);
		}
	}

	int64_t file_write(ACKFILE *file, const void *buffer, uint32_t size)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return -1;
		} else {
			return PHYSFS_writeBytes(file, buffer, size);
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

	void file_flush(ACKFILE * file)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
		} else {
			PHYSFS_flush(file);
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
