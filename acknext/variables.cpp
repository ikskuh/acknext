#include "acknext.h"


extern "C"
{

// Graphics variables
COLOR screen_color = { 0, 0, 128 };

// Time variables
var time_step = 0.0f;

// Scheduler variables
void * context;
var task_priority;

}
