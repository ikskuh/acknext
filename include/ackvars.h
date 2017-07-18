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
 * @brief A bitmask setting the currently enabled task groups.
 *
 * @default -1
 * @see task_mask
 */
extern int task_enabled;

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

/**
 * @ingroup scheduler
 *
 * The task which is currently executed. This allows changing the priority
 * and behaviour of the task.
 */
extern TASK task;

/**
 * @ingroup gui
 * @brief The root widget that will be rendered each frame.
 *
 * By default, this variable is set to @ref camera, so the default world will
 * be rendered.
 */
extern WIDGET * render_root;

/**
 * @ingroup engine
 * @brief Modifies the behaviour of the engine
 */
extern enum FLAGS engine_flags;



extern struct ENTITY * ACKCONST me;
extern struct ENTITY * ACKCONST my;

////////////////////////////////////////////////////////////////////////////////

#define _ACKKEY(name, key, scan) extern int key_##name;
#include "ackkeys.h"
#undef _ACKKEY

#define _ACKKEY(name, key, scan) extern void (*on_##name)(int keycode, int scancode);
#include "ackkeys.h"
#undef _ACKKEY

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif


#endif // _ACKNEXT_ACKVARS_
