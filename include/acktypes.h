#pragma once

#ifndef _ACKNEXT_ACKTYPES_
#define _ACKNEXT_ACKTYPES_

#include <stdint.h>
#include "ackcfg.h"

/**
 * @file
 * @brief This file contains all structure and enum declarations.
 **/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * The numeric type used by Acknext.
 *
 * This aliases to float.
 */
typedef float var;

/**
 * @brief An enumeration of flags that can be applied to
 *        engine objects. Some flags may be used only only
 *        on some objects or won't have any effect.
 */
enum FLAGS
{
    /**
     * A flag that says that no flags are set.
     */
    NONE = 0,

    /**
     * If set, the entity will be visible.
     **/
    VISIBLE = (1<<0),

    /**
     * If set, the entity won't collide with other entities.
     **/
    PASSABLE = (1<<1),

    /**
     * If set, the entity will not be touchable by the mouse.
     */
    UNTOUCHABLE = (1<<2),

    /**
     * If set, the entity will cast a shadow.
     */
    SHADOW = (1<<3),

    /**
     * Marks an entity as translucent and moves it to the
     * translucent render pass.
     */
    TRANSLUCENT = (1<<4),
};

/**
 * @brief An enumeration of possible error values.
 */
enum ERROR
{
    /**
     * No error has happened.
     */
    SUCCESS = 0,

    /**
     * The engine ran out of memory.
     */
    OUT_OF_MEMORY = 1,

    /**
     * An error within SDL2 has happened.
     */
    SDL_ERROR = 2,

    /**
     * The compilation of a script failed.
     */
    COMPILATION_FAILED = 3,
};

/**
 * An engine object handle.
 *
 * Handles are unique identifiers for engine objects that
 * won't become "dangling". Handles still can point to
 * non-existent objects.
 */
struct HANDLE
{
    union
    {
        struct
        {
            /**
             * The type of the referenced object.
             */
            int type : 6;

            /**
             * The id of the referenced object.
             */
            int id : 26;
        };
        int value : 32;
    };
};

/**
 * @brief An rgb color using 8 bit per color channel.
 */
struct COLOR
{
    /**
     * The red color component.
     */
    uint8_t red;

    /**
     * The green color component.
     */
    uint8_t green;

    /**
     * The blue color component.
     */
    uint8_t blue;
} __attribute__((packed));

struct ENTITY;

/**
 * @brief A level that contains a set of entities and some other settings.
 */
struct LEVEL
{
    /**
     * @brief The filename of this level.
     */
    char ACKCONST * ACKCONST filename;

    /**
     * @brief A pointer to a linked list of entities that will be created within this level.
     */
    struct ENTITY * ACKCONST entities;

    /**
     * @brief flags
     */
    enum FLAGS flags;
};

/**
 * @brief An opaque handle to a script.
 */
struct SCRIPT;


typedef struct LEVEL LEVEL;

typedef struct COLOR COLOR;

typedef struct HANDLE HANDLE;

typedef struct SCRIPT SCRIPT;

typedef enum FLAGS FLAGS;

typedef enum ERROR ERROR;

#ifdef __cplusplus
}
#endif

#endif // _ACKNEXT_ACKTYPES_
