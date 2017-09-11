#include <engine.hpp>

#include "core/config.hpp"

struct ackfile
{
public:
	virtual ~ackfile() = default;

	virtual int64_t read(void *buffer, uint32_t size) = 0;

	virtual int64_t write(const void *buffer, uint32_t size) = 0;

	virtual void seek(uint64_t position) = 0;

	virtual bool eof() = 0;

	virtual int64_t tell() = 0;

	virtual int64_t size() { return -1; } // Default is: "not implemented"

	virtual void flush() { } // Default: do nothing
};

struct physfile : public ackfile
{
private:
	FILE * file;
	size_t length;
public:
	physfile(FILE * file) :
	    file(file)
	{
		fseek(this->file, SEEK_END, 0);
		this->length = ftell(this->file);
		fseek(this->file, SEEK_SET, 0);
	}

	~physfile()
	{
		fclose(this->file);
	}

	virtual int64_t read(void *buffer, uint32_t size) override
	{
		return fread(buffer, 1, size, this->file);
	}

	virtual int64_t write(const void *buffer, uint32_t size) override
	{
		return fwrite(buffer, 1, size, this->file);
	}

	virtual void seek(uint64_t position) override
	{
		fseek(this->file, SEEK_SET, position);
	}

	virtual bool eof() override
	{
		return feof(this->file);
	}

	virtual int64_t tell() override
	{
		return ftell(this->file);
	}

	virtual int64_t size() override
	{
		return this->length;
	}

	virtual void flush() override
	{
		fflush(this->file);
	}
};

struct virtfile : public ackfile
{
private:
	PHYSFS_File * file;
public:
	virtfile(PHYSFS_File * file) :
	    file(file)
	{

	}

	~virtfile()
	{
		PHYSFS_close(this->file);
	}

	virtual int64_t read(void *buffer, uint32_t size) override
	{
		return PHYSFS_readBytes(this->file, buffer, size);
	}

	virtual int64_t write(const void *buffer, uint32_t size) override
	{
		return PHYSFS_writeBytes(this->file, buffer, size);
	}

	virtual void seek(uint64_t position) override
	{
		PHYSFS_seek(this->file, position);
	}

	virtual bool eof() override
	{
		return PHYSFS_eof(this->file);
	}

	virtual int64_t tell() override
	{
		return PHYSFS_tell(this->file);
	}

	virtual int64_t size() override
	{
		return PHYSFS_fileLength(this->file);
	}

	virtual void flush() override
	{
		PHYSFS_flush(this->file);
	}
};

struct blobfile : public ackfile
{
private:
	BLOB * blob;
	size_t pointer;
	bool append;
public:
	blobfile(BLOB * blob, bool append) :
	    blob(blob), pointer(0), append(append)
	{

	}

	~blobfile()
	{
		// this is kinda easy
	}

	virtual int64_t read(void * buffer, uint32_t size) override
	{
		int64_t len = this->calcActualLength(size);
		if(len > 0)
		{
			memcpy(buffer, this->dataPtr(), size_t(len));
		}
		return len;
	}

	virtual int64_t write(const void *buffer, uint32_t size) override
	{
		int64_t len = this->calcActualLength(size);
		if(len < size && this->append) {
			len = size; // this should do now!
			blob_resize(this->blob, this->pointer + size);
		}
		if(len > 0)
		{
			memcpy(this->dataPtr(), buffer, size_t(len));
		}
		return len;
	}

	virtual void seek(uint64_t position) override
	{
		this->pointer = size_t(position);
	}

	virtual bool eof() override
	{
		return (this->pointer >= this->blob->size);
	}

	virtual int64_t tell() override
	{
		return int64_t(this->pointer);
	}

	virtual int64_t size() override
	{
		return this->blob->size;
	}

	virtual void flush() override
	{
		// EASY MODE!
	}
private:
	int64_t calcActualLength(int64_t size) const
	{
		size_t end = this->pointer + size_t(size);
		if(end >= this->blob->size) {
			size = this->blob->size - this->pointer;
		}
		return size;
	}

	void * dataPtr() const
	{
		return reinterpret_cast<uint8_t*>(blob->data) + this->pointer;
	}
};

ACKNEXT_API_BLOCK
{
	ACKFILE * file_open_blob(BLOB * blob, bool allowResize)
	{
		ARG_NOTNULL(blob, nullptr);
		return new blobfile(blob, allowResize);
	}

	ACKFILE * file_open_read(char const * fileName)
	{
		ARG_NOTNULL(fileName,nullptr);
		if(engine_config.flags & USE_VFS) {
			PHYSFS_File * handle = PHYSFS_openRead(fileName);
			if(!handle)
				return nullptr;
			else
				return new virtfile(handle);
		} else {
			FILE * handle = fopen(fileName, "rb");
			if(!handle) {
				engine_seterror(ERR_FILESYSTEM, "The file '%s' could not be found!");
				return nullptr;
			}
			else
				return new physfile(handle);
		}
	}

	ACKFILE * file_open_write(char const * fileName)
	{
		ARG_NOTNULL(fileName,nullptr);
		if(engine_config.flags & USE_VFS) {
			PHYSFS_File * handle = PHYSFS_openWrite(fileName);
			if(!handle)
				return nullptr;
			else
				return new virtfile(handle);
		} else {
			FILE * handle = fopen(fileName, "wb");
			if(!handle) {
				engine_seterror(ERR_FILESYSTEM, "The file '%s' could not be found!");
				return nullptr;
			}
			else
				return new physfile(handle);
		}
	}

	ACKFILE * file_open_append(char const * fileName)
	{
		ARG_NOTNULL(fileName,nullptr);
		if(engine_config.flags & USE_VFS) {
			PHYSFS_File * handle = PHYSFS_openAppend(fileName);
			if(!handle)
				return nullptr;
			else
				return new virtfile(handle);
		} else {
			FILE * handle = fopen(fileName, "ab");
			if(!handle) {
				engine_seterror(ERR_FILESYSTEM, "The file '%s' could not be found!");
				return nullptr;
			}
			else
				return new physfile(handle);
		}
	}

	int64_t file_read(ACKFILE *file, void *buffer, uint32_t size)
	{
		ARG_NOTNULL(file, -1);
		return file->read(buffer, size);
	}

	int64_t file_write(ACKFILE *file, const void *buffer, uint32_t size)
	{
		ARG_NOTNULL(file, -1);
		return file->write(buffer, size);
	}

	void file_seek(ACKFILE * file, uint64_t position)
	{
		ARG_NOTNULL(file,);
		file->seek(position);
	}

	bool file_eof(ACKFILE * file)
	{
		ARG_NOTNULL(file, true);
		return file->eof();
	}

	int64_t file_tell(ACKFILE * file)
	{
		ARG_NOTNULL(file, 0);
		return file->tell();
	}

	int64_t file_size(ACKFILE * file)
	{
		ARG_NOTNULL(file, -1);
		return file->size();
	}

	void file_flush(ACKFILE * file)
	{
		ARG_NOTNULL(file,);
		file->flush();
	}

	void file_close(ACKFILE * file)
	{
		ARG_NOTNULL(file,);
		delete file;
	}
}
