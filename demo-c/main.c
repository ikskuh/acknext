#include <stdio.h>

#include <acknext.h>

#include <math.h>

void ackmain();

int main(int argc, char *argv[])
{
    if(engine_open(argc, argv) == false)
    {
        printf("Failed to initialize engine: %s\n", engine_lasterror(NULL));
        return 1;
    }
    start(ackmain, NULL);
    while(engine_frame())
    {
        // Now: Run!
    }
    engine_close();
    return 0;
}

void ackmain()
{
    float time = 0.0f;
    while(true)
    {
        screen_color.red = 128 + 127 * sin(time);
        screen_color.green = 128 + 127 * sin(time + 1.0);

        time += time_step;
        wait();
    }
}
