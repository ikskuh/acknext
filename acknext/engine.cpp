#include <acknext.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>

struct
{
    ERROR code;
    char message[1024];
} static lasterror = {
    SUCCESS, "Success.",
};

static std::chrono::steady_clock::time_point startupTime;

static FILE *logfile = nullptr;

ACKFUN bool engine_open(int argc, char ** argv)
{
    startupTime = std::chrono::steady_clock::now();
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-diag") == 0) {
            if(logfile != nullptr) {
                continue;
            }
            logfile = fopen("acklog.txt", "w");
            if(logfile == nullptr) {
                engine_log("Failed to open acklog.txt!");
            }
        }
    }



    engine_log("Engine ready.");

    return false;
}

ACKFUN bool engine_frame()
{
    return false;
}

ACKFUN void engine_close()
{
    engine_log("Shutting down engine...");

    engine_log("Engine shutdown complete.");
}

ACKFUN void engine_log(char const * format, ...)
{
    // TODO: Determine current time
    std::chrono::duration<float> timePoint = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startupTime);

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
    }
}

ACKFUN char const * engine_lasterror(ERROR * errcode)
{
    if(errcode != nullptr)
    {
        *errcode = lasterror.code;
    }
    return lasterror.message;
}

void engine_seterror(ERROR code, char const * message, ...)
{
    va_list args;
    va_start(args, message);
    vsnprintf(lasterror.message, 1023, message, args);
    va_end(args);

    lasterror.code = code;
}
