#include <engine.hpp>

#include <chrono>
#include "log.hpp"

FILE * logfile;
std::chrono::steady_clock::time_point startupTime;

ACKNEXT_API_BLOCK
{
	ACKFUN void engine_log(char const * format, ...)
	{
	    // TODO: Determine current time
	    std::chrono::duration<float> timePoint = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startupTime);

	    FILE * files[] = { stderr, logfile };

	    for(int i = 0; i < 2; i++)
	    {
	        if(files[i] == nullptr) {
	            continue;
	        }
	        fprintf(files[i], "%10.4f: ", timePoint.count());
	        va_list args;
	        va_start(args, format);
	        vfprintf(files[i], format, args);
	        va_end(args);
	        fprintf(files[i], "\n");
			fflush(files[i]);
	    }
	}
}
