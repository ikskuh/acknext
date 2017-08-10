#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void debug_tools();

void quit()
{
	engine_shutdown();
}

var randf(var min, var max)
{
	return (rand() / (var)RAND_MAX) * (max - min) + min;
}

void init(ENTITY * ent, MATERIAL * mtl, bool rot)
{
	vec_fill(&ent->scale, 10);
	ent->material = mtl;

	if(rot) {
		ent->rotation = *euler(
			randf(0, 360),
			randf(0, 360),
			randf(0, 360));
	}
}

MATERIAL * mtl_from_folder(char const * folder)
{
	char buffer[256];

	MATERIAL * mtl = mtl_create();

	sprintf(buffer, "%s/albedo.tif", folder);
	mtl->colorTexture = bmap_to_mipmap(bmap_load(buffer));

	sprintf(buffer, "%s/normal.tif", folder);
	mtl->normalTexture = bmap_to_mipmap(bmap_load(buffer));

	sprintf(buffer, "%s/attributes.tif", folder);
	mtl->attributeTexture = bmap_to_mipmap(bmap_load(buffer));

	mtl->roughness = 0.2;
	mtl->fresnell = 0.3;
	mtl->metallic = 0.0;

	return mtl;
}

void gamemain()
{
	filesys_addResource("/home/felix/projects/acknext/scripts/", "/");

	view_create((void*)render_scene_with_camera, camera);
	task_defer(debug_tools, NULL);
	event_attach(on_escape, quit);

	MATERIAL * mtl = mtl_from_folder("mtl_hardwood");

	init(ent_create("unit-sphere.obj",   vector(-15,  15, -50), NULL), mtl, true);
	init(ent_create("unit-cylinder.obj", vector(-15, -15, -50), NULL), mtl, true);
	init(ent_create("unit-sphere.obj",   vector( 15,  15, -50), NULL), mtl, true);
	init(ent_create("unit-cylinder.obj", vector( 15, -15, -50), NULL), mtl, true);

	for(int z = 0; z < 6; z++)
	{
		for(int x = -6; x <= 6; x++)
		{
			init(ent_create("unit-cube.obj", vector(10 * x, -25, -10 * z), NULL), mtl, false);
		}
	}

	while(true)
	{
		draw_point3d(vector(0, 10, -30), &COLOR_RED);
		task_yield();
	}
}

int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

