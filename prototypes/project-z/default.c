#include <acknext.h>

var debug_movement = 10;
var debug_speedup = 250;
var debug_slowdown = 9;

bool debug_camera_movement_enabled = false;

static void toggleMovement(void *x)
{
	(void)x;
	debug_camera_movement_enabled = !debug_camera_movement_enabled;
	beep();
}

static void toggleColliderDebug(void*x)
{
	(void)x;
	debug_collision = !debug_collision;
}

static void switchDebugMode(void*x)
{
	(void)x;
	opengl_debugMode = (opengl_debugMode + 1) % 4;
}

static void toggleWFrame(void*x)
{
	(void)x;
	opengl_wireFrame = !opengl_wireFrame;
}

void debug_tools()
{
	var pan = 0;
	var tilt = 0;
	event_attach(on_f8, toggleWFrame);
	event_attach(on_f9, switchDebugMode);
	event_attach(on_f10, toggleColliderDebug);
	event_attach(on_0, toggleMovement);
	while(!key_escape)
	{
		if(debug_camera_movement_enabled) {
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
			vec_normalize(&mov, (debug_movement + debug_speedup * key_lshift - debug_slowdown * key_lalt) * time_step);
			vec_rotate(&mov, &camera->rotation);
			vec_add(&camera->position, &mov);
		}
		task_yield();
	}
}
