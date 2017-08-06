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
		if(signbit(value)) value += max - min;
		value += min;
		return value;
	}

	var oscillate(var value, var min, var max)
	{
		var range = max - min;
		value -= min;
		value = fmod(value, (range * 2.0f));
		if (signbit(value)) value += 2.0f * range;
		if (value >= range) value = 2.0f * range - value;
		value += min;
		return value;
	}
}
