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

	ent_create("earth.mdl", vector(0, 0, 0), NULL);

	for(int x = -4; x <= 4; x++) {
		for(int z = -4; z <= 4; z++) {
			ENTITY * ent = ent_create("earth.mdl", vector(16 * x, 0, 16 * z), NULL);
			vec_fill(&ent->scale, 0.25);
		}
	}

	var pan = 0;
	var tilt = 0;
	while(!key_escape)
	{
		pan -= 0.3 * mickey.x;
		tilt -= 0.3 * mickey.y;

		QUATERNION qpan, qtilt;

		quat_axis_angle(&qpan, vector(0, 1, 0), pan);
		quat_axis_angle(&qtilt, vector(1, 0, 0), tilt);

		quat_set(&camera->rotation, &qpan);
		quat_mult(&camera->rotation, &qtilt);

		VECTOR mov = {
			key_d - key_a,
		    key_e - key_q,
		    key_s - key_w,
		};
		vec_normalize(&mov, (10 + 20 * key_lshift) * time_step);

		vec_rotate(&mov, &camera->rotation);

		vec_add(&camera->position, &mov);

		task_yield();
	}

	engine_shutdown();
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

