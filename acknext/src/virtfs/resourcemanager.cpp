#include "resourcemanager.hpp"

#include <physfs.h>

struct dummy;

extern dummy _binary_resource_zip_start;
extern dummy _binary_resource_zip_end;
// extern unsigned char _binary_resource_zip_size;

void ResourceManager::initialize()
{
	void * start = &_binary_resource_zip_start;
	size_t length = (uintptr_t)&_binary_resource_zip_end - (uintptr_t)&_binary_resource_zip_start;
	PHYSFS_mountMemory(
		start,
		length,
		nullptr,
		"resource.zip",
		"/builtin",
		1);
}

void ResourceManager::shutdown()
{

}
