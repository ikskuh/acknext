#include "engine.hpp"
#include <assert.h>
#include <acknext/serialization.h>

#define SIMPLE_IMPL(_name, _type) \
	_type file_read_##_name(ACKFILE * file) { \
		_type result; \
		assert(file_read(file, &result, sizeof(_type)) == sizeof(_type)); \
		return result; \
	} \
	void file_write_##_name(ACKFILE * file, _type value) { \
		assert(file_write(file, &value, sizeof(_type)) == sizeof(_type)); \
	}

ACKNEXT_API_BLOCK
{
	bool file_read_signature(ACKFILE * file, void const * reference, size_t length)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL!");
			return false;
		}
		if(length == 0) {
			return true; // zero length signatures are inherently valid :P
		}
		if(reference == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "reference must not be NULL!");
			return false;
		}

		// assume a signature is a short piece ;)
		uint8_t signature[length];
		assert(file_read(file, signature, length) == int(length));

		return !memcmp(signature, reference, length);
	}

	SIMPLE_IMPL(int8,   int8_t)
	SIMPLE_IMPL(int16,  int16_t)
	SIMPLE_IMPL(int32,  int32_t)
	SIMPLE_IMPL(int64,  int64_t)

	SIMPLE_IMPL(uint8,  uint8_t)
	SIMPLE_IMPL(uint16, uint16_t)
	SIMPLE_IMPL(uint32, uint32_t)
	SIMPLE_IMPL(uint64, uint64_t)

	SIMPLE_IMPL(float,  float)
	SIMPLE_IMPL(double,  double)

	SIMPLE_IMPL(vector2, VECTOR2)
	SIMPLE_IMPL(vector,VECTOR)
	SIMPLE_IMPL(vector4,VECTOR4)

	SIMPLE_IMPL(quat,QUATERNION)
	SIMPLE_IMPL(matrix,MATRIX)

	SIMPLE_IMPL(size,SIZE)
	SIMPLE_IMPL(point,POINT)

	SIMPLE_IMPL(uv,UVCOORD)
	SIMPLE_IMPL(color,COLOR)

	SIMPLE_IMPL(guid, ACKGUID)

	// returns malloc'ed!, length=0→varlen
	char * file_read_string(ACKFILE * file, int length)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL!");
			return nullptr;
		}
		if(length < 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "length must not be negative!");
			return nullptr;
		}

		// read length from file
		if(length == 0) {
			length = file_read_uint32(file);
		}
		char * result = (char*)malloc(length + 1);
		memset(result, 0, length + 1);

		assert(file_read(file, result, length) == length);

		return result;
	}

	void file_write_string(ACKFILE * file, char const * text, int maxLength)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL!");
			return;
		}
		if(text == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "text must not be NULL!");
			return;
		}
		if(maxLength < 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "maxLength must not be negative!");
			return;
		}

		if(maxLength == 0)
		{
			uint32_t length = strlen(text);
			file_write_uint32(file, length);
			file_write(file, text, length);
		}
		else
		{
			void * data = malloc(maxLength);
			memset(data, 0, maxLength);
			strncpy((char*)data, text, maxLength);
			file_write(file, data, maxLength);
			free(data);
		}
	}
}
