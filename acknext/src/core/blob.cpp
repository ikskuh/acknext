#include "blob.hpp"

Blob::Blob(size_t size) : EngineObject<BLOB>()
{
	api().data = malloc(size);
	api().size = size;
}

Blob::~Blob()
{
	free(api().data);
	api().data = nullptr;
}

void Blob::resize(size_t size)
{
	api().data = realloc(api().data, size);
	api().size = size;
}

ACKNEXT_API_BLOCK
{
	BLOB * blob_create(size_t size)
	{
		return demote(new Blob(size));
	}

	BLOB * blob_load(char const * fileName)
	{
		if(fileName == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "fileName must not be NULL!");
			return nullptr;
		}
		ACKFILE * file = file_open_read(fileName);
		if(file == nullptr) {
			engine_seterror(ERR_FILESYSTEM, "The given file `%s` was not found.", fileName);
			return nullptr;
		}

		int64_t size = file_size(file);
		if(size < 0) {
			file_close(file);
			return nullptr;
		}

		BLOB * blob = blob_create(size_t(size));
		file_read(file, blob->data, 1, blob->size);
		file_close(file);

		return blob;
	}

	BLOB * blub_clone(BLOB const * blob)
	{
		Blob const * b = promote<Blob>(blob);
		if(b) {
			Blob * n = new Blob(b->api().size);
			memcpy(n->api().data, b->api().data, b->api().size);
			return demote(n);
		} else {
			engine_seterror(ERR_INVALIDARGUMENT, "blob must not be NULL!");
			return nullptr;
		}
	}

	void blob_resize(BLOB * blob, size_t size)
	{
		Blob * b = promote<Blob>(blob);
		if(b) {
			b->resize(size);
		} else {
			engine_seterror(ERR_INVALIDARGUMENT, "blob must not be NULL!");
		}
	}

	void blob_remove(BLOB * blob)
	{
		Blob * b = promote<Blob>(blob);
		if(b) {
			delete b;
		}
	}
}
