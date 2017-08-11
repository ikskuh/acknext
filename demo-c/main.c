#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>

void debug_tools();

VECTOR cursor3d;

VECTOR previous;

var randv(var lower, var upper)
{
	return ((var)rand() / (var)RAND_MAX) * (upper-lower) + lower;
}

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

		selectedLight->position.x += 16 * (key_kp_6 - key_kp_4) * time_step;
		selectedLight->position.y += 16 * (key_kp_9 - key_kp_3) * time_step;
		selectedLight->position.z += 16 * (key_kp_2 - key_kp_8) * time_step;

		if(key_kp_2 || key_kp_3 || key_kp_4 || key_kp_6 || key_kp_8 || key_kp_9) {
			engine_log("(%f %f %f)",
				selectedLight->position.x,
				selectedLight->position.y,
				selectedLight->position.z);
		}

		draw_point3d(&selectedLight->position, &COLOR_WHITE);
	}
}

void makeLight()
{
	for(int i = 1; i < 10; i++)
	{
		if(lights[i]) continue;
		lights[i] = light_create(POINTLIGHT);
		lights[i]->position = camera->position;
		lights[i]->color = (COLOR){!!(i&1), !!(i&2), !!(i&4), 1 };
		lights[i]->intensity = 16;
		break;
	}
}

void torchwood(LIGHT * light)
{
	while(true)
	{
		var prev = light->intensity;
		var next = 16 + randv(-1, 1);
		var time = randv(0.08, 0.15);
		for(var f = 0; f < 1.0; f += (1.0 / time) * time_step)
		{
			light->intensity = lerp(prev, next, f);
			task_yield();
		}
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

	LIGHT * playerAura = light_create(POINTLIGHT);
	playerAura->color = *color_hex(0xffde96);
	playerAura->intensity = 8;

	lights[0] = playerAura;

	event_attach(on_l, nextLight);
	event_attach(on_k, makeLight);

	task_defer(controlLight, NULL);

	float scene[] =
	{
	    -40, 26, -51,
		 40, 26, -51,
		-60, 26, -71,
	     60, 26, -71,
	    -60, 26, -135,
	     60, 26, -135,
	    -40, 26, -157,
		 40, 26, -157,
	     -8, 26, -270,
	      8, 26, -270,
	};
	for(int i = 0; i < 10; i++)
	{
		LIGHT * torch = light_create(POINTLIGHT);
		torch->intensity = 16;
		torch->color = *color_hex(0xffd196);
		torch->position.x = scene[3*i+0];
		torch->position.y = scene[3*i+1];
		torch->position.z = scene[3*i+2];
		task_defer(torchwood, torch);
	}

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

