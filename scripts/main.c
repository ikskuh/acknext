#include <acknext.h>

#include <GL/gl3w.h>

action rotator()
{
	while(true)
	{
		wait();
	}
}

void main()
{
    ENTITY * ent = ent_create("earth.mdl", (VECTOR){ 0, 0, 0 }, &rotator);

	screen_color.blue = 0.5;

	camera->position = vector(25, 0, 0);
	// camera->widget.position = (POINT){200, 150};
	// camera->widget.size = (SIZE){400, 300};

	while(1)
	{
		// camera->x -= 10.0 * time_step;
		camera->rotation = euler(0, 0, 45.0 * total_secs);
		wait();
	}
}
