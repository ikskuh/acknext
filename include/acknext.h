#pragma once

#ifndef _ACKNEXT_
#define _ACKNEXT_

/**
 * @file
 * @brief This is the main include file for the engine.
 *
 * This file contains all function declarations that are being used in acknext.
 *
 *
 */

#include "ackcfg.h"
#include "acktypes.h"
#include "ackvars.h"

#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates a new, empty level.
 * @return
 */
ACKFUN LEVEL * level_create();

/**
 * @brief Loads a level from the given filename.
 * @param filename The filename that contains the level.
 * @return
 */
ACKFUN LEVEL * level_load(char const * filename);

/**
 * @brief Destroys a level and removes all contained entities.
 * @param level
 */
ACKFUN void level_remove(LEVEL *level);

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Yields the current coroutine and waits a single update frame.
 *
 * @remarks This function only works in coroutines.
 */
ACKFUN void task_wait();

/**
  * @brief start Starts a new coroutine.
  * @param fn      The coroutine function
  * @param context A user context that can be used to pass specific information+
  *                to the coroutine.
  *
  * Starts an asynchronous, cooperative process that will run in parallel
  * to other tasks.
  *
  * Each task must call @ref task_wait when it's work for the current
  * frame is done.
  *
  * @remarks The default @ref task_priority is 0.0
  * @remarks Each task will copy the priority of its creating task.
  * @remarks Tasks are executed in the order of their @ref task_priority.
  */
ACKFUN void task_start(void (*fn)(), void * context);

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the engine.
 *
 * @returns true when the engine was successfully initialized.
 * @remarks No engine function should be called before this is beeing called.
 * @remarks This function should only be called when not using the engines
 * internal main loop.
 */
ACKFUN bool engine_open(int argc, char ** argv);

/**
 * @brief Updates the engine and renders the next frame.
 *
 * @returns false when the engine was beeing shut down.
 */
ACKFUN bool engine_frame();

/**
 * @brief Shuts down the engine
 * @remarks This function should only be called when not using the engines
 * internal main loop.
 */
ACKFUN void engine_close();

/**
 * @brief Writes a message to the engine log.
 * @param format The format string of the message
 * @param ...    Variadic arguments for message formatting.
 */
ACKFUN void engine_log(char const * format, ...);

/**
 * @brief Gets the name of the last engine error that occurred.
 * @param errorcode Optional parameter that will receive the error
 *                  code of error.
 * @returns Textual description of the error.
 */
ACKFUN char const * engine_lasterror(ERROR * errorcode);

////////////////////////////////////////////////////////////////////////////////

#ifndef _ACKNEXT_INTERNAL_

// Those defines allow the use of wait() and start() instead
// of sched_wait() and sched_start()
#define wait task_wait
#define start task_start

// This define allows the use of a typed context variable
#define CONTEXT(Type) (*((Type*)context))

#endif

#endif // _ACKNEXT_
