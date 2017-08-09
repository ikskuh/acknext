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

void gamemain()
{
	filesys_addResource("packed.zip", "/packed.zip/");

	view_create((void*)render_scene_with_camera, camera);

	ENTITY * ent = ent_create("earth.mdl", vector(0, 0, 0), NULL);
	vec_fill(&ent->scale, 0.125);
	ent->material = mtl_create();
	ent->material->colorTexture = bmap_load("/packed.zip/packed.png");

	for(int x = -4; x <= 4; x++) {
		for(int z = -4; z <= 4; z++) {
			ENTITY * ent = ent_create("earth.mdl", vector(16 * x, 0, 16 * z), NULL);
			vec_fill(&ent->scale, 0.25);
			ent->rotation = *euler(
				360.0 * rand() / (var)RAND_MAX,
				360.0 * rand() / (var)RAND_MAX,
				0);

			if(x||z) hull_createBox(ent, vector(5, 5, 5));

			task_defer(rotor, ent);
		}
	}

	task_yield();

	// run debug tools :)
	task_defer(debug_tools, NULL);

	event_attach(on_mouse_left, paint);

	var trace = 0;
	while(!key_escape)
	{
		cursor3d = (VECTOR){ mouse_pos.x, mouse_pos.y, 64 };
		vec_for_screen(&cursor3d, NULL, NULL);

		COLLISION * c;
		if(c = c_trace(&camera->position, &cursor3d, BITFIELD_ALL))
		{
			vec_set(&cursor3d, &c->contact);
		}

		ent->position = cursor3d;

		task_yield();
	}

	engine_shutdown();
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

