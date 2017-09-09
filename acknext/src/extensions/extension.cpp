#include "extension.hpp"

#include <assert.h>
#include <acknext/serialization.h>

std::list<Extension> Extension::extensions;

Extension::Extension(std::string const & name, EXTENSION * ext) :
    name(name),
    ext(ext)
{
	assert(ext != nullptr);
}

void Extension::writeHeader(ACKFILE * file, ACKTYPE type, ACKGUID const & guid)
{
	file_write(file, "ACKNEXT", 8);
	file_write(file, &ACKNEXT_MAGIC, 4);
	file_write_uint32(file, type);
	file_write_guid(file, guid);
}

void * Extension::load(ACKFILE * file, ACKTYPE refType)
{
	if(file == nullptr) {
		return nullptr;
	}
	if(!file_read_signature(file, "ACKNEXT", 8))
		return nullptr;
	if(!file_read_signature(file, &ACKNEXT_MAGIC, sizeof(ACKNEXT_MAGIC)))
		return nullptr;

	ACKTYPE type = (ACKTYPE)file_read_uint32(file);
	ACKGUID guid = file_read_guid(file);

	if(refType != type) {
		engine_seterror(ERR_INVALIDOPERATION, "The file does not match the requested type!");
		return nullptr;
	}

	for(Extension & ext : extensions)
	{
		if(ext.ext->canLoad(&guid) != type)
			continue;
		switch(type)
		{
#define X(_type, _func) case TYPE_##_type: \
				if(ext.ext->load##_func) { \
					return (_type*)ext.ext->load##_func(file, &guid); \
				} else { \
					continue; \
				}
			X(MODEL, Model)
			X(SHADER, Shader)
			X(MATERIAL, Material)
			X(SOUND, Sound)
			// X(MUSIC, Music)
			X(HULL, Hull)
			X(BLOB, Blob)
			X(BITMAP, Bitmap)
			X(BUFFER, Buffer)
			X(MESH, Mesh)
			X(VIEW, View)
			X(LIGHT, Light)
			default: abort();
#undef X
		}
	}

	engine_seterror(ERR_INVALIDOPERATION, "The provided file does not have a fitting extension");
	return nullptr;
}

ACKNEXT_API_BLOCK
{
	bool ext_register(const char *name, EXTENSION *extension)
	{
		if(name == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "name must not be NULL");
			return false;
		}
		if(extension == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "extension must not be NULL");
			return false;
		}
		Extension::extensions.emplace_back(name, extension);
		return true;
	}
}

#define X(_type) \
	template<> _type * Extension::load<_type>(ACKFILE * file) { \
		return (_type*)load(file, TYPE_##_type ); \
	}
X(MODEL)
X(SHADER)
X(MATERIAL)
X(SOUND)
// X(MUSIC)
X(HULL)
X(BLOB)
X(BITMAP)
X(BUFFER)
X(MESH)
X(VIEW)
X(LIGHT)
#undef X
