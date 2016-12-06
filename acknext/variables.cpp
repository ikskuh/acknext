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

// Scheduler variables
void * context;
var task_priority;

}
