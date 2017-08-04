#include <stdio.h>
#include <acknext.h>

#include <GL/gl3w.h>

void draw_something(COLOR * cc)
{
	glClearColor(cc->red,cc->green,cc->blue,cc->alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void print_something()
{
	engine_log("mouse state: %d %d %d %d %d",
		mouse_left,
	   mouse_middle,
	   mouse_right,
	   mouse_x1,
	   mouse_x2);
}

void print_keydown()
{
	engine_log("keypress:\t%d\t'%s'", key_lastpressed, str_for_key(key_lastpressed));
}

int main(int argc, char *argv[])
{
    if(engine_open(argc, argv) == false)
    {
		fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror_text);
        return 1;
    }

	VIEW * view = view_create(draw_something, &COLOR_CYAN);

	view = view_create(draw_something, &COLOR_RED);
	view->flags &= ~FULLSCREEN;
	view->position = (POINT) { 10, 10 };
	view->size = (SIZE) { 100, 100 };

	view = view_create(draw_something, &COLOR_GREEN);
	view->flags &= ~FULLSCREEN;

	event_attach(on_mouse_left, print_something);
	event_attach(on_mouse_right, print_something);
	event_attach(on_mouse_middle, print_something);
	event_attach(on_mouse_x1, print_something);
	event_attach(on_mouse_x2, print_something);

	event_attach(on_anykey, print_keydown);

    while(engine_frame())
    {
		view->position = (POINT){ 0.25 * screen_size.width, 0.3 * screen_size.height };
		view->size = (SIZE){ 0.5 * screen_size.width, 0.4 * screen_size.height };
        // Now: Run!
    }

    engine_close();
    return 0;
}

