#include <stdio.h>
#include <acknext.h>

#include <GL/gl3w.h>

void draw_something(COLOR * cc)
{
	glClearColor(cc->red,cc->green,cc->blue,cc->alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void tasker(int * data)
{
	engine_log("task entry");
	while(!key_space)
	{
		if(key_b) task_kill(NULL);
		if(key_c) task_exit();
		task_yield();
	}
}

void failure(int * data)
{
	*data = 2;
}
void success(int * data)
{
	*data = 1;
}

int main(int argc, char *argv[])
{
	if(engine_open(argc, argv) == false)
    {
		fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror_text);
        return 1;
    }

	view_create(draw_something, &COLOR_CYAN);

	int value = 0;
	TASK * task = task_defer(tasker, &value);

	event_attach(task->finished, success);
	event_attach(task->failed, failure);

    while(engine_frame())
    {
		engine_log("Task state: %d", value);
        // Now: Run!
    }

    engine_close();
    return 0;
}

