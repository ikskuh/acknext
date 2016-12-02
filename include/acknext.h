#pragma once

#include "ackcfg.h"
#include "acktypes.h"


#if defined(__cplusplus)
extern "C"
{
#endif

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates a new, empty level.
 * @return
 */
LEVEL * level_create();

/**
 * @brief Loads a level from the given filename.
 * @param filename The filename that contains the level.
 * @return
 */
LEVEL * level_load(char const * filename);

/**
 * @brief Destroys a level and removes all contained entities.
 * @param level
 */
void level_remove(LEVEL *level);

////////////////////////////////////////////////////////////////////////////////




#if defined(__cplusplus)
}
#endif
