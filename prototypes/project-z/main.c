#include <acknext.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl3w.h>

#include <stdlib.h>

#include <assert.h>

#include <default.h>
#include <terrainmodule.h>

void storepos()
{
	if(key_lctrl)
	{
		ACKFILE * file = file_open_write("camera.dat");
		file_write(file, camera, sizeof(CAMERA));
		file_close(file);
	}
}

#define	GL_TEXTURE_MAX_ANISOTROPY_EXT          0x84FE
#define	GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT      0x84FF


void funnylight()
{
	LIGHT * funlight = light_create(SPOTLIGHT);
	funlight->color = COLOR_WHITE;
	funlight->intensity = 25.0;
	funlight->arc = 40;
	funlight->position = camera->position;
	funlight->direction = *vec_rotate(vector(0,0,-1), &camera->rotation);

	while(true)
	{
		draw_point3d(&funlight->position, &COLOR_RED);
		draw_line3d(
			&funlight->position,
			vec_add(
				vec_clone(&funlight->position),
				vec_normalize(vec_clone(&funlight->direction), 10)),
			&COLOR_GREEN);
		task_yield();
	}
}

void tree()
{
	const int radius = 250;
	const int count = 250;
	const VECTOR center = camera->position;
	for(int i = 0; i < count; i++)
	{
		VECTOR off;
		off.x = center.x + (rand() / (float)RAND_MAX) * 2*radius - radius;
		off.z = center.z + (rand() / (float)RAND_MAX) * 2*radius - radius;
		// off.y = l3hf_get(hf, off.x, off.z) - 0.2;
		ent_create("/models/tree.amd", &off, NULL);
		task_yield();
	}
}

void gamemain()
{
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	engine_log("max anisotropy: %f", fLargest);

	view_create((RENDERCALL)render_scene_with_camera, camera);
	filesys_addResource("/home/felix/projects/acknext/prototypes/project-z/resources/", "/");

	event_attach(on_s, storepos);
	event_attach(on_t, tree);
	event_attach(on_l, funnylight);

	terrainmodule_init();
	default_init();
	default_speedup = 500;

	{
		ACKFILE * file = file_open_read("camera.dat");
		if(file) {
			file_read(file, camera, sizeof(CAMERA));
			file_close(file);
		}
	}

	ent_create("/terrain/GrassyMountains.terrain", vector(0,0,0), NULL);

	LIGHT * sun = light_create(SUNLIGHT);
	sun->direction = *vec_normalize(vector(0.6, -1, -0.4), 1.0);
	sun->color = *color_hex(0xfffac1);

	LIGHT * ambi = light_create(AMBIENTLIGHT);
	ambi->color = *color_hex(0x232c33);

	var pan = 0;
	var tilt = 0;
	while(true)
	{
		if(key_n) {
			sun->color = COLOR_BLACK;
		} else {
			sun->color = *color_hex(0xfffac1);
		}
		if(key_4) {
			engine_log("%f ms / %f FPS", 1000.0 * time_step, 1.0 / time_step);
		}

		if(!default_camera_movement_enabled)
		{
			pan -= 0.3 * mickey.x;
			tilt = clamp(tilt - 0.3 * mickey.y, -80, 85);

			camera->rotation = *euler(pan, tilt, 0);

			VECTOR mov = {
				key_d - key_a,
				0,
				key_s - key_w,
			};
			vec_normalize(&mov, (3 + 3 * key_lshift) * time_step);
			vec_rotate(&mov, euler(pan, 0, 0));
			vec_add(&camera->position, &mov);

			// camera->position.y = l3hf_get(hf, camera->position.x, camera->position.z) + 1.8;
		}
		task_yield();
	}

}
