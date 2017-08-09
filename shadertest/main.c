#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>

void debug_tools();

void quit()
{
	engine_shutdown();
}

void gamemain()
{
	view_create((void*)render_scene_with_camera, camera);
	task_defer(debug_tools, NULL);
	event_attach(on_escape, quit);

	MATERIAL * mtl = mtl_create();

	ent_create("sphere.obj", vector(-15,  15, -50), NULL)->material = mtl;
	ent_create("sphere.obj", vector(-15, -15, -50), NULL)->material = mtl;
	ent_create("sphere.obj", vector( 15,  15, -50), NULL)->material = mtl;
	ent_create("sphere.obj", vector( 15, -15, -50), NULL)->material = mtl;

	while(true)
	{
		if(mouse_left) {
			mtl->roughness = (var)mouse_pos.x / (var)screen_size.width;
			mtl->metallic = (var)mouse_pos.y / (var)screen_size.height;
		}
		draw_point3d(vector(0, 10, -30), &COLOR_RED);
		task_yield();
	}
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

