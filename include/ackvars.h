#pragma once

#ifndef _ACKNEXT_ACKVARS_
#define _ACKNEXT_ACKVARS_

#include "ackcfg.h"
#include "acktypes.h"

/**
 * @file
 * @brief This file contains all the global engine variables.
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * The background color of the engine window.
 */
extern COLOR screen_color;


/**
 * The duration of the last frame in seconds.
 *
 * A speed multiplied by time_step gives the distance to be covered per frame.
 */
extern var ACKCONST time_step;

/**
 * The time in seconds since the start of the game.
 */
extern var ACKCONST total_secs;

/**
 * The number of frames that were already rendered.
 */
extern int ACKCONST total_frames;

/**
 * The context of the current task passed to @ref task_start.
 */
extern void * ACKCONST context;

/**
 * The priority of the current task.
 *
 * This variables allows to schedule tasks to your will.
 * Tasks with lower priority will come before tasks with higher priority.
 */
extern var task_priority;

#ifdef __cplusplus
}
#endif


#endif // _ACKNEXT_ACKVARS_
