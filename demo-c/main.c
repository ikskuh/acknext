#include <stdio.h>
#include <acknext.h>

int main(int argc, char *argv[])
{
    if(engine_open(argc, argv) == false)
    {
        fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror(NULL));
        return 1;
    }

    while(engine_frame())
    {
        // Now: Run!
    }

    engine_close();
    return 0;
}

