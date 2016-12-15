#include <acknext.h>

/*
int main(int argc, char ** argv)
{
    video_settitle("Acknext - Render Pipeline Demo");

    engine_open(argc, argv);

    LEVEL * level = level_create();

    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);
    ent_attach(ent, level);

    /*
    STAGE * stage = stage_create();
    stage->level = level;

    VIEW * view = camera; // default view
    camera->stage = stage;
    camera->position = (VECTOR){ -50, 0, 0 };
    *

    level_remove(level);

    engine_close();
}

*/

void khit()
{
    engine_log("k was hit!");
}

void main()
{
    engine_log("Hello World!");
    on_k = khit;
    while(true)
    {
        screen_color.red = 128 + 127 * sin(total_secs);

        screen_color.green = key_g * 255;

        wait();
    }
}
