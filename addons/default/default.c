#include <acknext.h>

var default_movement =  10;
var default_speedup  = 250;
var default_slowdown =   1;

bool default_camera_movement_enabled = false;

static void toggleMovement(void *x)
{
	(void)x;
	default_camera_movement_enabled = !default_camera_movement_enabled;
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

// on f4 or escape
static void defaultShutdown(void*x)
{
	(void)x;
	if(key_lalt) // alt-escape or alt-f4 kills the engine
		engine_shutdown();
}

static void default_loop()
{
	var pan = 0;
	var tilt = 0;
	while(true)
	{
		if(default_camera_movement_enabled) {
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
			var speed = default_movement;
			if(key_lalt) {
				speed = default_slowdown;
			} else if(key_lshift) {
				speed = default_speedup;
			}

			vec_normalize(&mov, speed * time_step);
			vec_rotate(&mov, &camera->rotation);
			vec_add(&camera->position, &mov);
		}
		task_yield();
	}
}

void default_init()
{
	task_defer((ENTRYPOINT)default_loop, NULL);

	event_attach(on_f4,  defaultShutdown);
	event_attach(on_f8,  toggleWFrame);
	event_attach(on_f9,  switchDebugMode);
	event_attach(on_f10, toggleColliderDebug);
	event_attach(on_f12, toggleMovement);
	event_attach(on_escape, defaultShutdown);
}
