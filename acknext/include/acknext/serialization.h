#ifndef _ACKNEXT_SERIALIZATION_H_
#define _ACKNEXT_SERIALIZATION_H_

#include "../acknext.h"

#define ACKSER_DECL(_name, _type) \
	ACKFUN _type file_read_##_name(ACKFILE * file); \
	ACKFUN void  file_write_##_name(ACKFILE * file, _type value);

ACKSER_DECL(int8,   int8_t)
ACKSER_DECL(int16,  int16_t)
ACKSER_DECL(int32,  int32_t)
ACKSER_DECL(int64,  int64_t)

ACKSER_DECL(uint8,  uint8_t)
ACKSER_DECL(uint16, uint16_t)
ACKSER_DECL(uint32, uint32_t)
ACKSER_DECL(uint64, uint64_t)

ACKSER_DECL(float,  float)
ACKSER_DECL(double,  double)

ACKSER_DECL(vector2, VECTOR2)
ACKSER_DECL(vector,VECTOR)
ACKSER_DECL(vector4,VECTOR4)

ACKSER_DECL(quat,QUATERNION)
ACKSER_DECL(matrix,MATRIX)

ACKSER_DECL(size,SIZE)
ACKSER_DECL(point,POINT)

ACKSER_DECL(uv,UVCOORD)
ACKSER_DECL(color,COLOR)

ACKSER_DECL(guid, ACKGUID)

ACKFUN bool file_read_signature(ACKFILE * file, void const * signature, size_t length);

ACKFUN char * file_read_string(ACKFILE * file, int length); // returns malloc'ed!, length=0→varlen

// maxLength = 0 → dynamic length
ACKFUN void file_write_string(ACKFILE * file, char const * text, int maxLength);

ACKFUN void file_write_symlink(ACKFILE * file, char const * referencedFile, bool useCaching);

#endif // _ACKNEXT_SERIALIZATION_H_
