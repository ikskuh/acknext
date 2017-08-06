#include <engine.hpp>

ACKNEXT_API_BLOCK
{
	// vec→viewspace→screenspace
	VECTOR * vec_to_screen(VECTOR * vec, CAMERA * camera, VIEW * view)
	{
		if(vec == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "vec must not be NULL!");
			return nullptr;
		}
	}

	// vec→viewspace→worldspace
	VECTOR * vec_for_screen(VECTOR * vec, CAMERA * camera, VIEW * view)
	{

	}
}
