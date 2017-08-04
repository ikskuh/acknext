#include <engine.hpp>

#include <strings.h>

#define STR(x) #x

ACKNEXT_API_BLOCK
{
	char const * str_for_key(int scancode)
	{
		switch(scancode)
		{
#define _ACKNEXT_KEYDEF(_name,_scancode) case _scancode: return #_name;
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF
			default:
				engine_seterror(ERR_INVALIDARGUMENT, "scancode %d is not valid!", scancode);
				return "Unknown";
		}
	}

	int key_for_string(char const * keyname)
	{
		if(keyname == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "keyname must not be NULL!");
			return 0;
		}
#define _ACKNEXT_KEYDEF(_name,_scancode) if(strcasecmp(keyname, #_name) == 0) return _scancode;
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF
		return 0;
	}
}
