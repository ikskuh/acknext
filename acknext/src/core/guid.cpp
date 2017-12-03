#include <engine.hpp>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

ACKNEXT_API_BLOCK
{
	ACKGUID guid_parse(char const * text)
	{
		ACKGUID result;
		if(*text == '{') text++;
		for(int i = 0; i < 16; i++)
		{
			int len, val;
			sscanf(text, "%02X%n", &val, &len);
			result.id[i] = (uint8_t)val;
			text += len;
			if(*text == '-') text++;
		}
		return result;
	}

	bool guid_compare(ACKGUID const * a, ACKGUID const * b)
	{
		return !memcmp(a, b, sizeof(ACKGUID));
	}
}
