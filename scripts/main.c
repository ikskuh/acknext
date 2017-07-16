#include <acknext.h>

void main()
{
    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);

	camera->position = vector(0, 0, -10);
	camera->widget.position = (POINT){200, 150};
	camera->widget.size = (SIZE){400, 300};
}
