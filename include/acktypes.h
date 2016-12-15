#pragma once

#ifndef _ACKNEXT_ACKTYPES_
#define _ACKNEXT_ACKTYPES_

#include <stdint.h>
#include <GL/gl.h>
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

    /**
     * Marks an entity as transient and prevents deletion
     * of the entity when the level containing the entity
     * gets destroyed.
     */
    TRANSIENT = (1<<5),
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

/**
 * @brief A two-dimensional, integral position value
 */
struct POINT
{
    /**
     * The horizontal component
     */
    int x;

    /**
     * The vertical component
     */
    int y;
}  __attribute__((packed));

/**
 * @brief A two-dimensional, integral size value
 */
struct SIZE
{
    /**
     * The horizontal component
     */
    int width;

    /**
     * The vertical component
     */
    int height;
}  __attribute__((packed));


/**
 * @brief A 3d vector
 */
struct VECTOR
{
    /**
     * @brief x coordinate
     */
    var x;

    /**
     * @brief y coordinate
     */
    var y;

    /**
     * @brief z coordinate
     */
    var z;
} __attribute__((packed));

/**
 * @brief A matrix type used for calculations.
 */
typedef float MATRIX[4][4];

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
     * @brief flags
     */
    enum FLAGS flags;

#ifdef _ACKNEXT_INTERNAL_
    struct LEVELdetail * _detail;
#endif
};

/**
 * @ingroup scene
 * @brief An entity is an object in a 3d world.
 */
struct ENTITY
{
    union
    {
        /**
         * @brief The position of the entity either in world coordinates
         *        or in local coordinates relative to its parent
         *
         * @see parent
         */
        struct VECTOR position;
        struct {
            /**
             * @brief x-component of the @ref position.
             */
            var x;

            /**
             * @brief y-component of the @ref position.
             */
            var y;

            /**
             * @brief z-component of the @ref position.
             */
            var z;
        };
    };

    /**
     * @brief The parent of the entity.
     *
     * Entities without parent will be rendered at their given @brief position.
     * Entities that will have a parent assigned treat their position relative
     * to the position of their parent.
     */
    struct ENTITY * parent;

    /**
     * @brief Flags modifying the behaviour of the entitiy.
     */
    enum FLAGS flags;
};

/**
 * @brief An opaque handle to a script.
 */
struct SCRIPT;

/**
 * @brief A gui widget that will be rendered in 2d.
 */
struct WIDGET
{
    /**
     * @brief Horizontal position of the
     */
    union {
        struct {
            /**
             * @brief Horizontal relative position
             */
            int x;
            /**
             * @brief Vertical relative position
             */
            int y;
        };
        /**
         * @brief The position of the widget relative to its parents.
         */
        struct POINT position;
    };
    union {
        struct {
            /**
             * @brief Horizontal size in pixels.
             */
            int width;

            /**
             * @brief Vertical size in pixels.
             */
            int height;
        };
        /**
         * @brief The size of the widget in pixels.
         */
        struct SIZE size;
    };

    /**
     * @brief The parent of the widget.
     */
    struct WIDGET * ACKCONST parent;

    /**
     * @brief Flags modifying the behaviour of this widget.
     */
    enum FLAGS flags;
};

/**
 * @ingroup rendering
 * @brief A texture object
 *
 * A bitmap can be any kind of OpenGL texture, including renderbuffer objects.
 */
struct BITMAP
{
    GLenum ACKCONST target;
    GLint ACKCONST object;
    GLint ACKCONST format;
    GLint ACKCONST internalFormat;
};

/**
 * @ingroup rendering
 * @brief A render pipeline stage.
 */
struct STAGE
{
    /**
     * @brief The four render targets of this stage.
     *
     * This array contains the four targets of a rendering process.
     *
     * @remarks Each target must be distinct, no duplicates allowed.
     */
    struct BITMAP * targets[4];

    /**
     * @brief The depth buffer that will be used for this stage.
     */
    struct BITMAP * depthBuffer;

    /**
     * @brief The level this stage should render
     */
    struct LEVEL * level;

    /**
     * @brief A dependency of the stage. Can be used to chain stages together
     *
     * If a stage is beeing rendered, first its chain of dependencies will be
     * rendered. If two stages share a single dependency, the dependency stage
     * will only be rendered once.
     */
    struct STAGE * dependency;

    /**
     * @brief Flags modifying the stage behaviour
     */
    enum FLAGS flags;
};

/**
 * @brief The VIEW struct
 */
struct VIEW
{
    /**
     * @brief The widget definition of this view.
     *
     * This widget contains the 2d representation of the view.
     */
    struct WIDGET widget;

    union {
        struct {
            /**
             * @brief Coordinates of the view in 3d space.
             */
            var x, y, z;
        };
        /**
         * @brief Position of the view in 3d space.
         */
        struct VECTOR position;
    };

    /**
     * @brief The render pipeline stage that should be displayed by this
     *        view.
     */
    struct STAGE * stage;

    /**
     * @brief Flags modifying the behaviour of this view.
     */
    enum FLAGS flags;
};

/**
 * @ingroup scheduler
 * @brief A task started by the scheduler.
 *
 */
struct TASK
{
    /**
     * @brief The priority of the current task.
     *
     * This variables allows to schedule tasks to your will.
     * Tasks with lower priority will come before tasks with higher priority.
     */
    var priority;

    /**
     * @brief A bitmask setting the enable-groups of this task.
     *
     * @default -1
     * @remarks This variable is task-global
     * @see task_enable
     */
    int mask;

    /**
     * @brief The context of the passed to @ref task_start.
     */
    void * ACKCONST context;

    /**
     * @brief Flags modifying the behaviour of this task.
     */
    enum FLAGS flags;
};

/**
 * An engine object handle.
 *
 * Handles are unique identifiers for engine objects that
 * won't become "dangling". Handles still can point to
 * non-existent objects.
 */
typedef uint32_t HANDLE;

#define _EXPORT_STRUCT(name) typedef struct name name;

_EXPORT_STRUCT(ENTITY)
_EXPORT_STRUCT(LEVEL)
_EXPORT_STRUCT(COLOR)
_EXPORT_STRUCT(POINT)
_EXPORT_STRUCT(SIZE)
_EXPORT_STRUCT(SCRIPT)
_EXPORT_STRUCT(VIEW)
_EXPORT_STRUCT(VECTOR)
_EXPORT_STRUCT(WIDGET)
_EXPORT_STRUCT(ENTITY)
_EXPORT_STRUCT(STAGE)
_EXPORT_STRUCT(BITMAP)
_EXPORT_STRUCT(TASK)

typedef enum FLAGS FLAGS;

typedef enum ERROR ERROR;

#undef _EXPORT_STRUCT

#ifdef __cplusplus
}
#endif

#endif // _ACKNEXT_ACKTYPES_
