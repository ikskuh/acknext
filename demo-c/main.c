#include <stdio.h>
#include <acknext.h>

VECTOR cursor3d;

VECTOR previous;

void paint()
{
	ENTITY * ent = ent_create("earth.mdl", &cursor3d, NULL);
	vec_fill(&ent->scale, 0.25);

	VECTOR other = previous;
	previous = cursor3d;

	while(true)
	{
		draw_line3d(&other, &ent->position, &COLOR_RED);
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

			if(x||z) hull_createSphere(ent, 2.5);
		}
	}

	task_yield();

	event_attach(on_mouse_left, paint);

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
			ent->position = cursor3d;
		}

		VECTOR rot = { 10, 0, 0};
		for(var i = 0; i < 360; i += 10)
		{
			draw_point3d(&rot, &COLOR_GREEN);
			vec_rotate(&rot, euler(10, 0, 0));
		}

		for(var i = 0; i < 32; i += 1)
		{
			draw_point3d(vec_rotate(vector(i, 0, 0), euler(trace, 0, 0)), &COLOR_GREEN);
		}

		COLLISION * col = c_trace(
			&nullvector,
			vec_rotate(vector(32, 0, 0), euler(trace, 0, 0)),
			BITFIELD_ALL);
		if(col != NULL) {
			draw_line3d(
				&nullvector,
				&col->contact,
				&COLOR_MAGENTA);
		}

		trace += 10 * time_step;

		task_yield();
	}

	engine_shutdown();
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

