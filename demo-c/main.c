#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>

void debug_tools();

VECTOR cursor3d;

VECTOR previous;

void paint()
{
	ENTITY * ent = ent_create("earth.mdl", &cursor3d, NULL);
	vec_fill(&ent->scale, 0.25);
	hull_createSphere(ent, 2.5);

	VECTOR other = previous;
	previous = cursor3d;

	while(true)
	{
		draw_line3d(&other, &ent->position, &COLOR_RED);
		task_yield();
	}
}

void rotor(ENTITY * ent)
{
	while(true)
	{
		if(key_r) quat_mult(&ent->rotation, euler(45 * time_step, 30 * time_step, 15 * time_step));
		task_yield();
	}
}

void quit()
{
	engine_shutdown();
}

void gamemain()
{
	view_create((void*)render_scene_with_camera, camera);

	ENTITY * ent = ent_create("mtltest/dungeon.obj", vector(0, 0, 0), NULL);
	ent->material = mtl_create();
	ent->material->colorTexture = bmap_load("mtltest/dungeon-diffuse.png");
	ent->material->emissionTexture = bmap_load("mtltest/dungeon-emissive.png");
	ent->material->attributeTexture = bmap_load("mtltest/dungeon-attributes.png");
	ent->material->emission = (COLOR){2,2,2,2};
	ent->material->roughness = 1.0;
	ent->material->metallic = 1.0;
	ent->material->fresnell = 25.0;

	task_yield();

	// run debug tools :)
	task_defer(debug_tools, NULL);

	event_attach(on_escape, quit);

	while(true)
	{
		task_yield();
	}
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

