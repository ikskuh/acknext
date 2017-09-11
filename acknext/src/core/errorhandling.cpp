#include <engine.hpp>

#include <stdarg.h>
#include <stdio.h>

#ifdef __linux__
#include <execinfo.h>
// int backtrace(void **buffer, int size);
// char **backtrace_symbols(void *const *buffer, int size);
void _print_stacktrace()
{
	void * stack[64];
	int cnt = backtrace(stack, 64);

	char ** symbols = backtrace_symbols(stack, cnt);
	if(symbols == nullptr) {
		for(int i = 3; i < cnt; i++) {
			engine_log("\t%d [%p]", i - 2, stack[i]);
		}
	} else {
		for(int i = 3; i < cnt; i++) {
			engine_log("\t%s", symbols[i]);
		}
	}
}

#else
#error "Implement stack tracing for other OS as well"
#endif

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

		_print_stacktrace();

		engine_lasterror_text = buffer;

		if(!(engine_config.flags & SILENT_FAIL)) {
			abort();
		}
	}
}

void engine_setsdlerror()
{
    engine_seterror(ERR_SDL, "%s", SDL_GetError());
}
