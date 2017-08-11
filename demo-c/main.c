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

LIGHT * selectedLight = NULL;
int lindex = 0;
LIGHT * lights[10] = { NULL };

void nextLight()
{
	do
	{
		lindex = (++lindex % 10);
	} while(!lights[lindex]);
	selectedLight = lights[lindex];
	engine_log("Current light: %d", lindex);
}

void controlLight(LIGHT * light)
{
	for(;;task_yield())
	{
		if(!selectedLight) continue;
		selectedLight->intensity += 16 * (key_kp_plus - key_kp_minus) * time_step;
		selectedLight->intensity = maxv(selectedLight->intensity, 0.5);

		selectedLight->position.x += 16 * (key_kp_4 - key_kp_6) * time_step;
		selectedLight->position.y += 16 * (key_kp_9 - key_kp_3) * time_step;
		selectedLight->position.z += 16 * (key_kp_2 - key_kp_8) * time_step;

		draw_point3d(&selectedLight->position, &COLOR_WHITE);
	}
}

void gamemain()
{
	view_create((void*)render_scene_with_camera, camera);

	ENTITY * ent = ent_create("mtltest/dungeon.obj", vector(0, 0, 0), NULL);
	ent->material = mtl_create();
	ent->material->colorTexture = bmap_load("mtltest/dungeon-diffuse.png");
	ent->material->emissionTexture = bmap_load("mtltest/dungeon-emissive.png");
	ent->material->attributeTexture = bmap_load("mtltest/dungeon-attributes.png");
	ent->material->normalTexture = bmap_load("mtltest/dungeon-normals.png");
	ent->material->emission = (COLOR){2,2,2,2};
	ent->material->roughness = 1.0;
	ent->material->metallic = 1.0;
	ent->material->fresnell = 25.0;

	task_yield();

	// run debug tools, first in the frame
	task_defer(debug_tools, NULL)->priority = -10;

	event_attach(on_escape, quit);

	for(int i = 1; i < 8; i++)
	{
		lights[i] = light_create(POINTLIGHT);
		lights[i]->color = (COLOR){!!(i&1), !!(i&2), !!(i&4), 1 };
		lights[i]->intensity = 16;
	}

	LIGHT * playerAura = light_create(POINTLIGHT);
	playerAura->color = *color_hex(0xffde96);
	playerAura->intensity = 8;

	lights[0] = playerAura;

	event_attach(on_l, nextLight);

	task_defer(controlLight, NULL);

	while(true)
	{
		playerAura->position = camera->position;
		task_yield();
	}
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

