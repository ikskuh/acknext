#pragma once

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
