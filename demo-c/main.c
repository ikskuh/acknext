#include <stdio.h>
#include <acknext.h>

#include <GL/gl3w.h>

void fail()
{

}

void gamemain()
{
	camera->position.z = 16;

	view_create(render_scene_with_camera, camera);

	ent_create("earth.mdl", vector(0, 0, -32), NULL);
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

