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
 * @ingroup video
 * The background color of the engine window.
 */
extern COLOR screen_color;

/**
 * @ingroup video
 * The current size of the window.
 */
extern SIZE ACKCONST screen_size;

/**
 * @ingroup time
 * The duration of the last frame in seconds.
 *
 * A speed multiplied by time_step gives the distance to be covered per frame.
 */
extern var ACKCONST time_step;

/**
 * @ingroup time
 * The time in seconds since the start of the game.
 */
extern var ACKCONST total_secs;

/**
 * @ingroup time
 * The number of frames that were already rendered.
 */
extern int ACKCONST total_frames;

/**
 * @ingroup scheduler
 * The context of the current task passed to @ref task_start.
 */
extern void * ACKCONST context;

/**
 * @ingroup scheduler
 * The priority of the current task.
 *
 * This variables allows to schedule tasks to your will.
 * Tasks with lower priority will come before tasks with higher priority.
 */
extern var task_priority;

/**
 * @ingroup gui
 * @brief The default view that renders the default level @ref world
 */
extern VIEW * camera;

/**
 * @ingroup scene
 * @brief The default level that will contain all created entities.
 */
extern LEVEL * world;

#ifdef __cplusplus
}
#endif


#endif // _ACKNEXT_ACKVARS_
