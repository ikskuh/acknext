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
#include "ackcpp.h"

#include <stddef.h>
#include <stdbool.h>
#include <math.h>

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
  * @returns A handle to the created task.
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
ACKFUN HANDLE task_start(void (*fn)(), void * context);

/**
 * @brief Terminates the given task.
 * @param htask A handle to the task that should be terminated.
 *
 * This function will end the task and release all of its resources.
 */
ACKFUN void task_kill(HANDLE htask);

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Compiles a script file and returns the script handle.
 * @param fileName The file containing the source code.
 * @returns Compiled script or NULL if errors happened.
 */
ACKFUN SCRIPT * script_compile(char const * fileName);

/**
 * @brief Compiles a script from source and returns the script handle.
 * @param source The source that should be compiled.
 * @returns Compiled script or NULL if errors happened.
 */
ACKFUN SCRIPT * script_compile_src(char const * source);

/**
 * @brief Gets an exported symbol from a script.
 * @param script The script that should contain the symbol.
 * @param name   The name of the symbol
 * @returns Pointer to the symbol or NULL if non-existent.
 */
ACKFUN void *script_symbol(SCRIPT * script, char const * name);

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

/**
 * @brief This is an alias to @ref task_wait
 */
#define wait task_wait

/**
 * @brief This is an alias to @ref task_start
 */
#define start task_start

/**
 * @brief This macro dereferences and casts the @ref context pointer to the given type
 *
 * This allows a simple use of the context variable: `CONTEXT(ENTITY*) = ent_create(...);`.
 */
#define CONTEXT(Type) (*((Type*)context))

#endif

#endif // _ACKNEXT_
