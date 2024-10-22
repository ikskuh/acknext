#include <acknext.h>
/*
action rotator()
{
	while(true)
	{
		quat_set(&my->rotation, euler(45.0 * total_secs, 0, 0));
		my->y = 14 * sin(total_secs + atan2(my->x, my->z));

		wait();
	}
}

// Collision system
void coltest()
{
	// benötigt collider-welt, da mehr als ein LEVEL möglich
	// falls NULL, wird ::world verwendet
	COLLISION const * hit = c_trace(from, to, -1, flags);

	hit->contact;  // VECTOR(position)
	hit->distance; // var(distance)
	hit->normal;   // VECTOR(normal)
	hit->other;    // ENTITY*('you')
	hit->flags;    // HITFLAGS(info)

	// verwendet alle LEVEL, an die 'me' gebunden ist
	COLLISION const * hit = c_move(me, absdist, reldist, flags);
}

void main()
{
	for(int i = 0; i < 12; i++) {
		var x = 50 * sin(DEG_TO_RAD * i * 30);
		var z = 50 * cos(DEG_TO_RAD * i * 30);
		ent_create("earth.mdl", vector(x, 0, z), &rotator);
	}

	MATERIAL * mtlRail = mtl_create();
	mtlRail->colorTexture = bmap_load("palette.png");

	ENTITY * ent;
	for(int i = 0; i < 3; i++) {
		ent = ent_create("models/tile_rail_long.obj", vector(0, -1, -3 - i), NULL);
		ent->material = mtlRail;
		quat_set(&ent->rotation, euler(0, 0, 0));
	}
	ent = ent_create("models/tile_rail_corner.obj", vector(0, -1, -6), NULL);
	ent->material = mtlRail;
	quat_set(&ent->rotation, euler(0, 0, 0));

	var pan = 0.0;
	var tilt = 0.0;

	while(true)
	{
		pan += 90.0 * (key_cul - key_cur) * time_step;
		tilt += 90.0 * (key_cuu - key_cud) * time_step;
		camera->rotation = *euler(pan, tilt, 0);
		camera->z -= 1.0 * (key_w - key_s) * time_step;
		camera->x += 1.0 * (key_d - key_a) * time_step;
		engine_log("%f %f", pan, tilt);
		wait();
	}
}
*/
