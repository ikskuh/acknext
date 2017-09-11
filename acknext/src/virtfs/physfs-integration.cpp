#include <engine.hpp>

#include <physfs.h>
#include "core/config.hpp"

ACKNEXT_API_BLOCK
{
	void filesys_addResource(char const * resource, char const * path)
	{
		if(!(engine_config.flags & USE_VFS)) {
			engine_seterror(ERR_INVALIDOPERATION, "acknext was not configured to use the virtual file system.");
			return;
		}
		ARG_NOTNULL(resource, );
		if(PHYSFS_mount(resource, path, 1) == 0) {
			engine_seterror(ERR_FILESYSTEM, "%s", PHYSFS_getLastError());
		}
	}
}
