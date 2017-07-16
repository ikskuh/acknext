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
 * @ingroup math
 * @brief The numeric type used by Acknext.
 *
 * This aliases to float.
 */
typedef float var;

/**
 * @ingroup engine
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
     * Marks a widget as automatically sized when the widgets
     * defined area is zero.
     *
     * @see WIDGET::size
     */
    AUTOSIZE = (1<<5),
};

/**
 * @ingroup engine
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

	/**
	 * An invalid argument was passed to a function.
	 */
	INVALID_ARGUMENT = 4,
};

/**
 * @ingroup gui
 * @brief An enumeration of @ref WIDGET types.
 */
enum WIDGETTYPE
{
    /**
     * The widget is of the type @ref VIEW.
     */
    WIDGET_VIEW = 1,

    /**
     * The widget is of the type @ref BUTTON.
     *
     * @todo Implement BUTTON widget.
     */
    WIDGET_BUTTON = 2,

    /**
     * The widget is of the type @ref IMAGE.
     *
     * @todo Implement IMAGE widget.
     */
    WIDGET_IMAGE = 3,

    /**
     * @brief The minimum type any user widget can have.
     *
     * If a widget type is larger than or equal to WIDGET_USER, it is a
     * widget type registered by @ref widget_registertype. This widget
     * type will be rendered and updated by the user script instead of the
     * engine itself.
     *
     * @todo Implement WIDGETTYPE widget_registertype(size_t size)
     */
    WIDGET_USER = 0x1000,
};

/**
 * @ingroup video
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
 * @ingroup math
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
 * @ingroup math
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
 * @ingroup math
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
 * @ingroup math
 * @brief A quaternion, used to represent rotations and orientations
 */
struct QUATERNION
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

    /**
     * @brief w coordinate
     */
    var w;
} __attribute__((packed));

/**
 * @ingroup math
 * @brief A matrix type used for calculations.
 */
typedef float MATRIX[4][4];

/**
 * @ingroup scene
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
	 * @brief The rotation of the entity.
	 */
	struct QUATERNION rotation;

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
 * @ingroup scripting
 * @brief An opaque handle to a script.
 */
struct SCRIPT;

/**
 * @ingroup gui
 * @brief A gui widget that will be rendered in 2d.
 */
struct WIDGET
{
    /**
     * @brief The engine type of the widget.
     */
    enum WIDGETTYPE ACKCONST type;

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
 * @ingroup gui
 * @brief A view to the 3d world.
 *
 * A view is a viewport and camera that will display a scene on the user
 * interface. It can render one or more levels (by the use of pipelines).
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
     * @brief The rotation of the view.
     *
     * The rotation will determine which direction is forward, left/right and
     * upwards.
     */
    struct QUATERNION rotation;

	/**
	 * @brief The field of view in vertical direction.
	 */
	var arc;

	/**
	 * @brief The aspect ration for width and height.
	 *
	 * This aspect ration is multiplied to the screen aspect ration
	 * and allows an image distortion.
	 */
	var aspect;

	/**
	 * @brief Depth of the short clipping plane.
	 */
	var zNear;

	/**
	 * @brief Depth of the far clipping plane.
	 */
	var zFar;

    /**
     * @brief The level that should be rendered by this view.
     *
     * @remarks This option is only used if @ref pipeline is not set.
     */
    struct LEVEL * level;

    /**
     * @brief The rendering pipeline that is invoked when rendering this view.
     *
     * @remarks If pipeline is set to NULL, the engine will invoke a simple renderer
     * that will draw the assigned LEVEL if any.
     */
    struct PIPELINE * pipeline;

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
     * @see task_enabled
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
_EXPORT_STRUCT(QUATERNION)
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

typedef enum WIDGETTYPE WIDGETTYPE;

#undef _EXPORT_STRUCT

#ifdef __cplusplus
}
#endif

#endif // _ACKNEXT_ACKTYPES_
