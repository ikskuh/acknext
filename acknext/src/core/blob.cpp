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
