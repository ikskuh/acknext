#pragma once

#include "ackcfg.h"
#include "acktypes.h"

/**
 * @file
 * @brief This file contains all the global engine variables.
 */

#if defined(__cplusplus)
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
extern var time_step;


extern void * ACKCONST context;

#if defined(__cplusplus)
}
#endif
