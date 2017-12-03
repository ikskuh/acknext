#include <stdio.h>
#include <acknext.h>
#include <acknext/ext/default.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl3w.h>
#include <math.h>

void update()
{

}

void init()
{
	filesys_addResource("/home/felix/projects/acknext/scripts/", "/");
	view_create((RENDERCALL)render_scene_with_camera, camera);
	default_init();
	event_attach(on_update, update);
}

int main(int argc, char ** argv)
{
	engine_config.argv0 = argv[0];
	return engine_main(init);
}
