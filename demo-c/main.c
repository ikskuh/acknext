#include <stdio.h>
#include <acknext.h>

#include <GL/gl3w.h>

void draw_something(COLOR * cc)
{
	glClearColor(cc->red,cc->green,cc->blue,cc->alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void key_event()
{
	for(int i = 0; i < 10; i++)
	{
		engine_log("Yay: %d", i);
		for(int j = 0; j < 60; j++) {
			task_yield();
		}
	}
}

void synchro()
{
	engine_log("Ich bin synchroooon!");
}

int main(int argc, char *argv[])
{
	if(engine_open(argc, argv) == false)
    {
		fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror_text);
        return 1;
    }

	VIEW * view = view_create(draw_something, &COLOR_CYAN);

	event_attach(on_space, key_event);

	if(task_start(synchro, NULL) ->state == TASK_DEAD) {
		engine_log("It worked!");
	}

    while(engine_frame())
    {
        // Now: Run!
    }

    engine_close();
    return 0;
}

