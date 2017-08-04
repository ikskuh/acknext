#include <engine.hpp>

#include <stdlib.h>
#include <math.h>

ACKNEXT_API_BLOCK
{
	var lerp(var lhs, var rhs, var factor)
	{
		return lhs * (var(1) - factor)
			+  rhs * factor;
	}

	var clamp(var value, var min, var max)
	{
		if(value < min) return min;
		if(value > max) return max;
		return value;
	}

	var cycle(var value, var min, var max)
	{
		value -= min;
		value =	fmod(value, max - min);
		value += min;
		return value;
	}
}
