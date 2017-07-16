#include "acknext.h"


extern "C"
{

// Graphics variables
COLOR screen_color = { 0, 0, 128 };

SIZE screen_size;

// Time variables
var time_step = 0.0f;
var total_secs = 0.0f;
int total_frames = 0;

int task_enabled = -1;

VIEW * camera;
LEVEL * world;

#define _ACKKEY(name, key, scan) int ACKCONST key_##name = 0;
#include <ackkeys.h>
#undef _ACKKEY

#define _ACKKEY(name, key, scan) void (*on_##name)(int keycode, int scancode);
#include <ackkeys.h>
#undef _ACKKEY

// Scheduler variables
#define TASKDATA __attribute__ ((section (".taskdata")))

TASK task TASKDATA;

#undef TASKDATA

}
