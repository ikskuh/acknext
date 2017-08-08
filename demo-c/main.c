#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>


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
		if(key_r) quat_mult(&ent->rotation, euler(45 * time_step, 0, 0));
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

			task_defer(rotor, ent);

			if(x||z) hull_createBox(ent, vector(5, 5, 5));
		}
	}

	task_yield();

	event_attach(on_mouse_left, paint);

	// debug_collision = true;

	var pan = 0;
	var tilt = 0;
	var trace = 0;
	while(!key_escape)
	{
		if(mouse_right) {
			pan -= 0.3 * mickey.x;
			tilt -= 0.3 * mickey.y;
		}

		camera->rotation = *euler(pan, tilt, 0);

		VECTOR mov = {
			key_d - key_a,
		    key_e - key_q,
		    key_s - key_w,
		};
		vec_normalize(&mov, (10 + 20 * key_lshift) * time_step);

		vec_rotate(&mov, &camera->rotation);

		vec_add(&camera->position, &mov);

		{
			cursor3d = (VECTOR){ mouse_pos.x, mouse_pos.y, 64 };
			vec_for_screen(&cursor3d, NULL, NULL);

			COLLISION * c;
			if(c = c_trace(&camera->position, &cursor3d, BITFIELD_ALL))
			{
				vec_set(&cursor3d, &c->contact);
			}

			ent->position = cursor3d;
		}
		task_yield();
	}

	engine_shutdown();
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

