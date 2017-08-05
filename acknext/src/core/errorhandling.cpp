#include <engine.hpp>

#include <stdarg.h>
#include <stdio.h>

#define STRINGIFY(x) #x

ACKNEXT_API_BLOCK
{
	ERROR ACKCONST engine_lasterror = ERR_NOERROR;

	char const * ACKCONST engine_lasterror_text = nullptr;

	void engine_seterror(ERROR code, const char *message, ...)
	{
		engine_lasterror = code;
		if(code == ERR_NOERROR) {
			engine_lasterror_text = nullptr;
			return;
		}

		static char buffer[512];
		va_list list;
		va_start(list, message);
		vsprintf(buffer, message, list);
		va_end(list);

		engine_log("Error: %s", buffer);

		engine_lasterror_text = buffer;
	}
}

void engine_setsdlerror()
{
    engine_seterror(ERR_SDL, "%s", SDL_GetError());
}
