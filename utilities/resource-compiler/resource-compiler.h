#ifndef _UTIL_RESOURCE_COMPILER_H_
#define _UTIL_RESOURCE_COMPILER_H_

#include <stddef.h>
#include <stdint.h>

typedef struct _resource_dummy _resource_dummy_t;

#define DECLARE_RESOURCE(fileName) \
	extern const _resource_dummy_t _binary_##fileName##_start; \
	extern const _resource_dummy_t _binary_##fileName##_end; \
// don't use the size symbol as it seemingly does not work...
// extern const size_t * _binary_##fileName##_size;

#define RESOURCE_SIZE(fileName) \
	((size_t)((intptr_t)&_binary_##fileName##_end - (intptr_t)&_binary_##fileName##_start))

#define RESOURCE_DATA(fileName) \
	((void*)&_binary_##fileName##_start)

#define RESOURCE_DATA_END(fileName) \
	((void*)&_binary_##fileName##_end)

#endif // _UTIL_RESOURCE_COMPILER_H_
