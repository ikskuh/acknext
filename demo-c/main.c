#include <stdio.h>

#include <acknext.h>

#include <math.h>
#include <stdlib.h>

void ackmain();
void turn_color();

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
    start(turn_color, &screen_color.red);
    screen_color.blue = 0;
    while(true)
    {
        // screen_color.red = 128 + 127 * sin(time);
        screen_color.green = 128 + 127 * sin(time + 1.0);

        time += time_step;
        wait();
    }
}

void turn_color()
{
    float time = 0.0;
    while(true)
    {
        CONTEXT(uint8_t) = 128 + 127 * sin(time);
        time += time_step;
        wait();
    }
}
