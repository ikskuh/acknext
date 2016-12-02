#include <stdio.h>

#include <acknext.h>

int main(int argc, char *argv[])
{
    if(engine_open(argc, argv) == false)
    {
        printf("Failed to initialize engine: %s\n", engine_lasterror(NULL));
        // return 1;
    }
    sleep(1);
    while(engine_frame())
    {
        // Now: Render!
    }
    engine_close();
    return 0;
}
