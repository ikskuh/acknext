#include <acknext.h>

void ackmain();
void turn_color();

void ackmain()
{
    start(turn_color, &screen_color.red);
    while(true)
    {
        screen_color.green = 128 + 127 * sinf(total_secs + 1.0);
        wait();
    }
}

void main()
{
    video_settitle("Acknext A1 - Demo");
    task_start(ackmain, NULL);
    while(true)
    {
        // Do some fancy screen coloring.
        for(screen_color.blue = 0; screen_color.blue < 255; screen_color.blue ++)
        {
            wait();
        }
    }
}

void turn_color()
{
    while(true)
    {
        CONTEXT(uint8_t) = 128 + 127 * sinf(total_secs);
        wait();
    }
}