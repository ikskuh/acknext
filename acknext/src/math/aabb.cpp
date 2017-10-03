#include <engine.hpp>

#include <stdlib.h>
#include <math.h>

ACKNEXT_API_BLOCK
{
	bool aabb_valid(AABB const * aabb)
	{
		ARG_NOTNULL(aabb, false);
#define OK(a) (aabb->maximum.a > aabb->minimum.a)
		return OK(x) && OK(y) && OK(z);
#undef OK
	}

	void aabb_invalidate(AABB * aabb)
	{
		ARG_NOTNULL(aabb,);
		aabb->minimum = (VECTOR) {  1,  1,  1 };
		aabb->maximum = (VECTOR) { -1, -1, -1 };
	}
}
