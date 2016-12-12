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
 * @ingroup gui scene
 * @brief Creates a new view.
 * @param parent The parent widget that will contain this view.
 * @return The newly created view.
 */
ACKFUN VIEW * view_create(WIDGET * parent);

/**
 * @ingroup gui scene
 * @brief Extracts the view and projection matrix from a view object.
 * @param view          The view which matrices should be extracted.
 * @param matView       A pointer to a matrix that should recevice
 *                      the view matrix.
 * @param matProjection A pointer to a matrix that should recevice
 *                      the projection matrix.
 */
ACKFUN void view_to_matrix(
    VIEW * view,
    MATRIX * matView,
    MATRIX * matProjection);

/**
 * @ingroup gui scene
 * @brief Destroys a view widget.
 * @param view The view that should be destroyed.
 */
ACKFUN void view_remove(VIEW * view);

////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup scene
 * @brief Creates a new, empty level.
 * @return
 */
ACKFUN LEVEL * level_create();

/**
 * @ingroup scene
 * @brief Loads a level from the given filename.
 * @param filename The filename that contains the level.
 * @return
 */
ACKFUN LEVEL * level_load(char const * filename);

/**
 * @ingroup scene
 * @brief Destroys a level
 * @param level
 */
ACKFUN void level_remove(LEVEL *level);

////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup scene
 * @brief Creates a new entity.
 * @param source   The entity file. Can either be a model file or
 *                 an entity definition file.
 * @param position The position where the entity should spawn
 * @param entmain  The main function that will be scheduled for the entity.
 * @returns        The newly created entity.
 */
ACKFUN ENTITY * ent_create(char const * source, VECTOR position, void (*entmain)());

/**
 * @ingroup scene
 * @brief Attaches en entity to the given level.
 * @param ent   The entity that should be attached
 * @param level The level where the entity will be attached.
 *
 * Inserts the entity in the given level.
 */
ACKFUN void ent_attach(ENTITY * ent, LEVEL * level);

/**
 * @ingroup scene
 * @brief Detaches an entity from the given level.
 * @param ent   The entity that should be detached.
 * @param level The level where the entity will be detached.
 *
 * Removes the entity from the given level.
 */
ACKFUN void ent_detach(ENTITY * ent, LEVEL * level);

/**
 * @ingroup scene
 * @brief Destroys an entity and removes it from all levels.
 * @param ent The entity to be destroyed
 */
ACKFUN void ent_remove(ENTITY * ent);

////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup video
 * @brief Sets the new video mode.
 * @param width      The width of the screen
 * @param height     The height of the screen
 * @param fullscreen If true, the engine window will go fullscreen.
 *
 * @remarks If width or height are 0, no change to the video resolution is done.
 */
ACKFUN void video_set(int width, int height, bool fullscreen);

/**
 * @ingroup video
 * @brief Sets the title of the engine window.
 * @param title
 */
ACKFUN void video_settitle(char const * title);

////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup scheduler
 * @brief Yields the current coroutine and waits a single update frame.
 *
 * @remarks This function only works in coroutines.
 */
ACKFUN void task_wait();

/**
 * @ingroup scheduler
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
 * @ingroup scheduler
 * @brief Terminates the given task.
 * @param htask A handle to the task that should be terminated.
 *
 * This function will end the task and release all of its resources.
 */
ACKFUN void task_kill(HANDLE htask);

////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup scripting
 * @brief Compiles a script file and returns the script handle.
 * @param fileName The file containing the source code.
 * @returns Compiled script or NULL if errors happened.
 */
ACKFUN SCRIPT * script_compile(char const * fileName);

/**
 * @ingroup scripting
 * @brief Compiles a script from source and returns the script handle.
 * @param source The source that should be compiled.
 * @returns Compiled script or NULL if errors happened.
 */
ACKFUN SCRIPT * script_compile_src(char const * source);

/**
 * @ingroup scripting
 * @brief Gets an exported symbol from a script.
 * @param script The script that should contain the symbol.
 * @param name   The name of the symbol
 * @returns Pointer to the symbol or NULL if non-existent.
 */
ACKFUN void *script_symbol(SCRIPT * script, char const * name);

////////////////////////////////////////////////////////////////////////////////

/**
 * @ingroup engine
 * @brief Initializes the engine.
 *
 * @returns true when the engine was successfully initialized.
 * @remarks No engine function should be called before this is beeing called.
 * @remarks This function should only be called when not using the engines
 * internal main loop.
 */
ACKFUN bool engine_open(int argc, char ** argv);

/**
 * @ingroup engine
 * @brief Updates the engine and renders the next frame.
 *
 * @returns false when the engine was beeing shut down.
 */
ACKFUN bool engine_frame();

/**
 * @ingroup engine
 * @brief Shuts down the engine
 * @remarks This function should only be called when not using the engines
 * internal main loop.
 */
ACKFUN void engine_close();

/**
 * @ingroup engine
 * @brief Writes a message to the engine log.
 * @param format The format string of the message
 * @param ...    Variadic arguments for message formatting.
 */
ACKFUN void engine_log(char const * format, ...);

/**
 * @ingroup engine
 * @brief Gets the name of the last engine error that occurred.
 * @param errorcode Optional parameter that will receive the error
 *                  code of error.
 * @returns Textual description of the error.
 */
ACKFUN char const * engine_lasterror(ERROR * errorcode);

/**
 * @ingroup engin
 * @ingroup scripting
 * @brief Returns a symbol name from any loaded script.
 * @param name The name of the symbol.
 * @return The first symbol that matches the given name in any loaded script.
 *
 * This function searches for a global script variable that can be located
 * in any loaded script.
 *
 * It will return the first occurance and ignores all later created symbols
 * with that name.
 */
ACKFUN void * engine_getscript(char const * name);

////////////////////////////////////////////////////////////////////////////////

#ifndef _ACKNEXT_INTERNAL_

// Those defines allow the use of wait() and start() instead
// of sched_wait() and sched_start()

/**
 * @ingroup scheduler
 * @brief This is an alias to @ref task_wait
 */
#define wait task_wait

/**
 * @ingroup scheduler
 * @brief This is an alias to @ref task_start
 */
#define start task_start

/**
 * @ingroup scheduler
 * @brief This macro dereferences and casts the @ref context pointer to the given type
 *
 * This allows a simple use of the context variable: `CONTEXT(ENTITY*) = ent_create(...);`.
 */
#define CONTEXT(Type) (*((Type*)context))

#endif

#endif // _ACKNEXT_
