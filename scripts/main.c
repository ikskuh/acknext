#include <acknext.h>

action rotator()
{
	while(true)
	{
		quat_set(&my->rotation, euler(45.0 * total_secs, 0, 0));
		my->z = 14 * sin(total_secs + atan2(my->x, my->y));
		wait();
	}
}

void main()
{
	for(int i = 0; i < 12; i++) {
		var x = 50 * sin(DEG_TO_RAD * i * 30);
		var y = 50 * cos(DEG_TO_RAD * i * 30);
		ent_create("earth.mdl", vector(x, y, 0), &rotator);
	}

	MATERIAL * mtlRail = mtl_create();
	mtlRail->colorTexture = bmap_load("palette.png");

	ENTITY * ent;
	for(int i = 0; i < 3; i++) {
		ent = ent_create("models/tile_rail_long.obj", vector(3 + i, 0, -1), NULL);
		ent->material = mtlRail;
		quat_set(&ent->rotation, euler(0, 0, 0));
	}
	ent = ent_create("models/tile_rail_corner.obj", vector(6, 0, -1), NULL);
	ent->material = mtlRail;
	quat_set(&ent->rotation, euler(180, 0, 0));

	while(true)
	{
		quat_set(&camera->rotation, euler(30.0 * total_secs, 0, 0));
		camera->x += 1.0 * time_step;
		wait();
	}
}
