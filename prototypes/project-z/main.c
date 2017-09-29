#include <acknext.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/gl3w.h>

#include <stdlib.h>

#include <assert.h>

#include <default.h>
#include <terrainmodule.h>
#include <ackcef.h>

static const bool enableCEF = true;

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

ENTITY * terrain;
SHADER * shdtree;

void spawn(const int radius, const int count, char const * fileName, float scale)
{
	const VECTOR center = camera->position;
	for(int i = 0; i < count; i++)
	{
		VECTOR off;
		off.x = center.x + (rand() / (float)RAND_MAX) * 2*radius - radius;
		off.z = center.z + (rand() / (float)RAND_MAX) * 2*radius - radius;
		off.y = terrain_getheight(terrain->model, off.x, off.z) - 0.2;
		ENTITY * tree = ent_create(fileName, &off, NULL);
		quat_axis_angle(&tree->rotation, vector(0,1,0), 360 * (float)rand() / (float)RAND_MAX);
		vec_fill(&tree->scale, scale);
	}
}

void tree()
{
	spawn(250, 250, "/models/tree.amd", 1.0);
}

void grass()
{
	spawn(50, 500, "/models/grass.amd", 0.4);
}

VIEW * cefview;

void outsider()
{
	static float time_step_over_time = 0;
	char buffer[128];

	time_step_over_time = 0.9 * time_step_over_time + 0.1 * time_step;

	int num = 0;
	ENTITY * you;
	for(you = ent_next(NULL); you; you = ent_next(you), num++);

	sprintf(buffer,
		"update(%.2f,%f,%d,%d)",
		camera->position.y,
		time_step_over_time,
		num,
		engine_stats.drawcalls);

	if(enableCEF) ackcef_exec(cefview, buffer);
}

bool nightmode = false;

static void toggle_nightmode()
{
	nightmode ^= true;
}

void gamemain()
{
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	engine_log("max anisotropy: %f", fLargest);

	view_create((RENDERCALL)render_scene_with_camera, camera);
	filesys_addResource("/home/felix/projects/acknext/prototypes/project-z/resources/", "/");
	filesys_addResource("/home/felix/projects/acknext/scripts/", "/other/");


	shdtree = shader_create();

//	if(shader_addFileSource(shdtree, VERTEXSHADER, "/builtin/shaders/object.vert") == false) {
	if(shader_addFileSource(shdtree, VERTEXSHADER, "/shaders/fastobject.vert") == false) {
		abort();
	}
//	if(shader_addFileSource(shdtree, FRAGMENTSHADER, "/shaders/fastobject.frag") == false) {
	if(shader_addFileSource(shdtree, FRAGMENTSHADER, "/builtin/shaders/object.frag") == false) {
		abort();
	}
	if(shader_addFileSource(shdtree, FRAGMENTSHADER, "/builtin/shaders/lighting.glsl") == false) {
		abort();
	}
	if(shader_addFileSource(shdtree, FRAGMENTSHADER, "/builtin/shaders/gamma.glsl") == false) {
		abort();
	}
	if(shader_addFileSource(shdtree, FRAGMENTSHADER, "/builtin/shaders/ackpbr.glsl") == false) {
		abort();
	}
	if(shader_addFileSource(shdtree, FRAGMENTSHADER, "/builtin/shaders/fog.glsl") == false) {
		abort();
	}
	if(shader_link(shdtree) == false) {
		abort();
	}


	if(enableCEF)
	{
		cefview = ackcef_createView();

		ackcef_navigate(cefview, "ack:///fungui.htm");

		while(!ackcef_ready(cefview))
		{
			task_yield();
		}

		ackcef_exec(cefview, "initialize()");
	}

	event_attach(on_s, storepos);
	event_attach(on_t, tree);
	event_attach(on_g, grass);
	event_attach(on_l, funnylight);
	event_attach(on_n, toggle_nightmode);

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

	terrain = ent_create("/terrain/GrassyMountains.terrain", vector(0,0,0), NULL);

	LIGHT * sun = light_create(SUNLIGHT);
	sun->direction = *vec_normalize(vector(0.6, -1, -0.4), 1.0);
	sun->color = *color_hex(0xfffac1);

	LIGHT * ambi = light_create(AMBIENTLIGHT);
	ambi->color = *color_hex(0x232c33);

	tree();

	ENTITY * player = ent_create("/other/cyber-pirate/cyber-pirate.amd", vector(0, 0, 512), NULL);

	vec_fill(&player->scale, 0.15);

	var pan = 0;
	var tilt = 0;
	while(true)
	{
		if(nightmode) {
			sun->color = COLOR_BLACK;
			ambi->color = *color_hex(0x090b0d);
		} else {
			sun->color = *color_hex(0xfffac1);
			ambi->color = *color_hex(0x232c33);
		}

		ent_animate(player, "Idle", total_time);

		if(key_3 && default_camera_movement_enabled)
		{
			player->position = camera->position;
		}

		if(!default_camera_movement_enabled)
		{
			pan -= 0.3 * mickey.x;
			tilt = clamp(tilt - 0.3 * mickey.y, -80, 85);

			camera->rotation = *euler(pan, tilt, 0);
			player->rotation = *euler(pan, 0, 0);

			VECTOR mov = {
				key_d - key_a,
				0,
				key_s - key_w,
			};
			vec_normalize(&mov, (3 + 3 * key_lshift) * time_step);
			vec_rotate(&mov, &player->rotation);
			vec_add(&player->position, &mov);

			player->position.y = terrain_getheight(terrain->model, player->position.x, player->position.z);

			camera->position = (VECTOR) { 0, 0, 5 };
			vec_rotate(&camera->position, &camera->rotation);
			vec_add(&camera->position, &player->position);
			vec_add(&camera->position, vector(0, 1.5, 0));

			var mincam = terrain_getheight(terrain->model, camera->position.x, camera->position.z) + 1.8;
			if(camera->position.y < mincam)
				camera->position.y = mincam;
		}


		if(key_4) {
			static float gpuTime = 0.0;
			static float frameTime = 0.0;

			gpuTime = 0.9 * gpuTime + 0.1 * engine_stats.gpuTime;
			frameTime = 0.9 * frameTime + 0.1 * time_step;

			int num = 0;
			ENTITY * you;
			for(you = ent_next(NULL); you; you = ent_next(you), num++);

			engine_log(
				"%8.4f ms / %8.4f ms GPU Time / %5d Objects / %5d Drawcalls / %8.4f FPS",
				1000.0 * frameTime,
				gpuTime,
				num,
				engine_stats.drawcalls,
				1.0 / frameTime);
		}

		outsider();
		task_yield();
	}
}

int main(int argc, char ** argv)
{
	// May not return!
	if(enableCEF) ackcef_init(argc, argv);

	assert(argc >= 1);
	engine_config.argv0 = argv[0];
	return engine_main(gamemain);
}
