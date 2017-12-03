#include <acknext.h>
#include <acknext/serialization.h>
#include <acknext/ext/terrain.h>
#include <acknext/ext/ackgui.h>
#include <acknext/ext/default.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl3w.h>

#include <stdlib.h>

#include <assert.h>

// #include <ackcef.h>

#ifdef _ACKNEXT_EXT_ACKCEF_H_
VIEW * cefview;
static const bool enableCEF = true;
#endif

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

	/*
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
	*/
}

ENTITY * terrain;

#define TERRAIN_MASK (1<<0)
#define PLAYER_MASK (1<<1)

static bool placeToGround(VECTOR * ref, float distance)
{
	COLLISION * hit = c_trace(
		vec_add(vec_clone(ref), vector(0, 10000, 0)),
		vec_add(vec_clone(ref), vector(0, -10000, 0)),
		TERRAIN_MASK);
	if(hit) {
		draw_point3d(&hit->contact, &COLOR_MAGENTA);
		ref->y = hit->contact.y + distance;
	}
	return (hit != nullptr);
}

void spawn(const int radius, const int count, char const * fileName, float scale, VECTOR const * center = nullptr)
{
	VECTOR middle;
	if(center)
		middle = *center;
	else
		middle = camera->position;

	for(int i = 0; i < count; i++)
	{
		VECTOR off;
		off.x = middle.x + (rand() / (float)RAND_MAX) * 2*radius - radius;
		off.z = middle.z + (rand() / (float)RAND_MAX) * 2*radius - radius;
		off.y = 0;
		ENTITY * tree = ent_create(fileName, &off, NULL);

		placeToGround(&tree->position, -0.15);

		quat_axis_angle(&tree->rotation, vector(0,1,0), 360 * (float)rand() / (float)RAND_MAX);
		vec_fill(&tree->scale, scale);
	}
}

void tree()
{
	spawn(250, 250 + key_lctrl * 1250, "/models/tree.amd", 1.0);
}

void house()
{
	spawn(0, 1, "/buildings/inn.amd", 0.1);
}

ENTITY * player;
bool nightmode = false;

bool editorMode = false;

ENTITY * entToInsert = NULL;

extern "C" void gui_drawperf();

void outsider()
{
#ifdef _ACKNEXT_EXT_ACKCEF_H_
	char buffer[128];
	sprintf(buffer,
		"update(%.2f,%f,%d,%d)",
		camera->position.y,
		time_step_over_time,
		num,
		engine_stats.drawcalls);

	ackcef_exec(cefview, buffer);
#endif

#ifdef _ACKNEXT_EXT_ACKGUI_H_

	gui_drawperf();

	if(ImGui::Begin("Camera / Player"))
	{
		ImGui::Text("Camera:");
		if(ImGui::Button("Save##Camera"))
		{
			ACKFILE * file = file_open_write("camera.dat");
			file_write(file, camera, sizeof(CAMERA));
			file_close(file);
		}
		ImGui::SameLine();
		if(ImGui::Button("Load##Camera"))
		{
			ACKFILE * file = file_open_read("camera.dat");
			file_read(file, camera, sizeof(CAMERA));
			file_close(file);
		}

		ImGui::Separator();

		ImGui::Text("Player:");
		if(ImGui::Button("Save##Player"))
		{
			ACKFILE * file = file_open_write("player.dat");
			file_write_vector(file, player->position);
			file_close(file);
		}
		ImGui::SameLine();
		if(ImGui::Button("Load##Player"))
		{
			ACKFILE * file = file_open_read("player.dat");
			player->position = file_read_vector(file);
			file_close(file);
		}
	}
	ImGui::End();

	if(editorMode)
	{
		if(ImGui::Begin("Level Editor", &editorMode))
		{
			static int selection = 0;
			static char const* items[] =
			{
				"/other/buildings/inn.amd",
				"/models/tree.amd",
			};
			static float scale[] =
			{
				0.0125,
				1.0,
			};
			ImGui::Text("Select Model:");
			if(ImGui::ListBox("##Spawn List", &selection, items, 2))
			{
				// Selection changed
			}
			if(ImGui::Button("Spawn"))
			{
				engine_log("Create %s", items[selection]);
				if(entToInsert) ent_remove(entToInsert);
				entToInsert = ent_create(items[selection], &camera->position, NULL);
				vec_fill(&entToInsert->scale, scale[selection]);
			}
		}
		ImGui::End();
	}
	else
	{
		if(entToInsert)
			ent_remove(entToInsert);
		entToInsert = NULL;
	}

	if(ImGui::Begin("Environment Control"))
	{
		ImGui::Checkbox("Night Scene", &nightmode);
		ImGui::SliderFloat("Exposure", &pp_exposure, 0.1, 10.0, "%.2f", 2.0);

		ImGui::Text("Features:");
		bool reinhard = (pp_stages & PP_REINHARD);
		bool ssao = (pp_stages & PP_SSAO);
		bool bloom = (pp_stages & PP_BLOOM);

		ImGui::Checkbox("Reinhard Tonemap", &reinhard);
		ImGui::Checkbox("SSAO", &ssao);
		ImGui::Checkbox("Bloom", &bloom);

		pp_stages = (pp_stages & ~(PP_REINHARD | PP_BLOOM | PP_SSAO))
			| (ssao ? PP_SSAO : 0)
			| (bloom ? PP_BLOOM : 0)
			| (reinhard ? PP_REINHARD : 0)
			;
	}
	ImGui::End();
#endif
}

void accept_insert()
{
	if(!editorMode)	return;
	entToInsert = NULL;
}

void cancel_insert()
{
	if(!editorMode)	return;

	if(entToInsert) {
		ent_remove(entToInsert);
		entToInsert = NULL;
	}
}

void toggle_editor()
{
	editorMode ^= true;
}

LIGHT *sun, *ambi, *plambilight;

void update()
{
	static var pan = 0;
	static var tilt = 0;
	static var camdist = 5;

	if(nightmode) {
		sun->color = COLOR_BLACK;
		ambi->color = *color_hex(0x090b0d);
	} else {
		sun->color = *color_hex(0xfffac1);
		ambi->color = *color_hex(0x232c33);
	}

	if(key_3 && default_camera_movement_enabled)
	{
		player->position = camera->position;
	}

	if(!default_camera_movement_enabled && !editorMode)
	{
		pan -= 0.3 * mickey.x;
		tilt = clamp(tilt - 0.3 * mickey.y, -80, 85);
		camdist = clamp(camdist - 0.5 * mickey.w, 2.0, 10.0);

		camera->rotation = *euler(pan, tilt, 0);

		VECTOR mov = {
			float(key_d - key_a),
			0,
			float(key_s - key_w),
		};

		vec_normalize(&mov, (3 + 3 * key_lshift - 2.5 * key_lalt) * time_step);
		vec_rotate(&mov, euler(pan, 0, 0));
		vec_add(&player->position, &mov);

		if(vec_length(&mov) > 0.0)
		{
			var offset = atan2(mov.x, mov.z);
			player->rotation = *euler(RAD_TO_DEG * offset, 0, 0);

			if(key_lshift)
				ent_animate(player, "Run", total_time);
			else
				ent_animate(player, "Walk", total_time);
		}
		else
		{
			ent_animate(player, "Idle", total_time);
		}

		placeToGround(&player->position, 0);

		camera->position = (VECTOR) { 0, 0, camdist };
		vec_rotate(&camera->position, &camera->rotation);
		vec_add(&camera->position, &player->position);
		vec_add(&camera->position, vector(0, 1.5, 0));

		VECTOR refpos = camera->position;
		placeToGround(&refpos, 0.2);

		if(camera->position.y < refpos.y)
			camera->position.y = refpos.y;

		vec_lerp(
			&plambilight->position,
			&player->position,
			&camera->position,
			0.4);
	}

	if(editorMode && entToInsert)
	{
		VECTOR from, to;
		from = { float(mouse_pos.x), float(mouse_pos.y), 0 };
		to   = { float(mouse_pos.x), float(mouse_pos.y), 100 };

		vec_for_screen(&from, camera, NULL);
		vec_for_screen(&to, camera, NULL);

		COLLISION * col = c_trace(&from, &to, BITFIELD_ALL);
		if(col)
		{
			draw_point3d(&col->contact, &COLOR_CYAN);
			if(entToInsert) entToInsert->position = col->contact;
		}
	}

	if(true)
	{
		for(int z = -25; z <= 25; z++)
		{
			for(int x = -25; x <= 25; x++)
			{
				var fx = player->position.x + 0.1 * x;
				var fz = player->position.z + 0.1 * z;
				COLLISION * c = c_trace(
					vector(fx, 1000, fz),
					vector(fx, -1000, fz),
					TERRAIN_MASK);
				if(c)
					draw_point3d(&c->contact, &COLOR_CYAN);
			}
		}
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
}

#define STR(x) #x

void gamemain()
{
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	engine_log("max anisotropy: %f", fLargest);

	view_create((RENDERCALL)render_scene_with_camera, camera);
	filesys_addResource(PROJECT_ROOT "/prototypes/project-z/resources/", "/");
	filesys_addResource(PROJECT_ROOT "/scripts/", "/other/");


#ifdef _ACKNEXT_EXT_ACKCEF_H_
	cefview = ackcef_createView();

	ackcef_navigate(cefview, "ack:///fungui.htm");

	while(!ackcef_ready(cefview))
	{
		task_yield();
	}

	ackcef_exec(cefview, "initialize()");
#endif

#ifdef _ACKNEXT_EXT_ACKGUI_H_
	ackgui_init();
#endif

	event_attach(on_t, (EVENTHANDLER)tree);
	event_attach(on_l, (EVENTHANDLER)funnylight);

	event_attach(on_f5, (EVENTHANDLER)toggle_editor);
	event_attach(on_mouse_left, (EVENTHANDLER)accept_insert);
	event_attach(on_mouse_right, (EVENTHANDLER)cancel_insert);

	terrainmodule_init();
	default_init();
	default_speedup = 500;
	default_camera_movement_enabled = true;

	{
		ACKFILE * file = file_open_read("camera.dat");
		if(file) {
			file_read(file, camera, sizeof(CAMERA));
			file_close(file);
		}
	}

	terrain = ent_create("/terrain.esd", vector(0,0,0), NULL);
	terrain->categories = TERRAIN_MASK;

	sun = light_create(SUNLIGHT);
	sun->direction = *vec_normalize(vector(0.6, -1, -0.4), 1.0);
	sun->color = *color_hex(0xfffac1);

	ambi = light_create(AMBIENTLIGHT);
	ambi->color = *color_hex(0x232c33);

	plambilight = light_create(POINTLIGHT);
	plambilight->intensity = 2;
	vec_fill((VECTOR*)&plambilight->color, 0.3);

	tree();

	player = ent_create("/other/cyber-pirate/cyber-pirate.amd", vector(0, 0, 0), NULL);
	player->categories = PLAYER_MASK;
	vec_fill(&player->scale, 0.15);

	hull_createBox(player, vector(1,2.5,1));

	ACKFILE * file = file_open_read("player.dat");
	if(file) {
		player->position = file_read_vector(file);
		file_close(file);
	}

	spawn(2048, 50000, "/models/tree.amd", 1.0, &nullvector);

	event_attach(on_update, (EVENTHANDLER)update);
}

int main(int argc, char ** argv)
{
	// May not return!
#ifdef _ACKNEXT_EXT_ACKCEF_H_
	ackcef_init(argc, argv);
#endif

	assert(argc >= 1);
	engine_config.argv0 = argv[0];
	return engine_main(gamemain);
}
