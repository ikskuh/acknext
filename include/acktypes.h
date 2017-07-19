#pragma once

#ifndef _ACKNEXT_ACKTYPES_
#define _ACKNEXT_ACKTYPES_

#include <stdint.h>
#include <stddef.h>
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
 * @brief action is an alias for the void type to provide compatibility
 *        to good ol' Gamestudio.
 */
typedef void action;

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

	/**
	 * When a uniform has this flag, its value will be automatically filled
	 * in by the engine.
	 *
	 * @see UNIFORM
	 */
	AUTOFILL = (1<<6),

	/**
	 * @ingroup rendering
	 * When this flag is set in @ref engine_flags, the
	 * engines internal rendering is completly disabled
	 * and the user must call everything manually.
	 *
	 * @see engine_swap
	 */
	CUSTOMDRAW = (1<<7),
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

	/**
	 * A function was called when it was not allowed to be called.
	 **/
	INVALID_OPERATION = 5,
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
 * @ingroup rendering
 * @brief An enumeration of shader types.
 */
enum SHADERTYPE
{
	/**
	 * A vertex shader
	 **/
	VERTEX_SHADER = 1,

	/**
	 * A fragment or pixel shader
	 **/
	FRAGMENT_SHADER = 2,
};

/**
 * @ingroup rendering
 * @brief An enumeration of buffer types.
 */
enum BUFFERTYPE
{
	/**
	 * A buffer for vertices.
	 * @see VERTEX
	 **/
	VERTEX_BUFFER = 1,

	/**
	 * A buffer for uint32 indices.
	 **/
	INDEX_BUFFER = 2,

	/**
	 * A buffer for uniform storage.
	 **/
	UNIFORM_BUFFER = 3,
};

/**
 * @ingroup rendering
 * @brief An enumeration of possible formats for bitmaps.
 */
enum PIXELFORMAT
{
	RGBA8,
	RGBA16F,
	RGBA32F,
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
    var red;

    /**
     * The green color component.
     */
    var green;

    /**
     * The blue color component.
     */
    var blue;

	/**
	 * The transparency component of the color.
	 */
	var alpha;
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
 * @brief A texture coord
 */
struct UVCOORD
{
    /**
     * @brief u coordinate
     */
    var u;

    /**
     * @brief y coordinate
     */
    var v;
} __attribute__((packed));

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
 * @brief A 4d vector
 */
struct VECTOR4
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
struct MATRIX
{
	var v[4][4];
};

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

	ACKNEXT_LEVEL_DETAIL ACKCONST * ACKCONST _detail;
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
	 * @brief Overrides the entities default material settings.
	 */
	struct MATERIAL * material;

    /**
     * @brief Flags modifying the behaviour of the entitiy.
     */
    enum FLAGS flags;

	ACKNEXT_ENTITY_DETAIL ACKCONST * ACKCONST _detail;
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
     * @brief Horizontal position of the widget.
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

/**
 * @ingroup rendering
 * @brief An OpenGL shader program
 */
struct SHADER;

/**
 * @brief An enumeration of predefined shader variables.
 */
enum SHADERVARIABLE
{
	UNDEFINED = 0,
	MAT_WORLD,
	MAT_VIEW,
	MAT_PROJ,
	MAT_VIEWPROJ,
	MAT_WORLDVIEW,
	MAT_WORLDVIEWPROJ,

	VEC_VIEWDIR,
	VEC_VIEWPOS,
	VEC_VIEWPORT,

	VEC_COLOR,
	VEC_EMISSION,
	VEC_ATTRIBUTES,
	TEX_COLOR,
	TEX_ATTRIBUTES,
	TEX_EMISSION,
};

/**
 * @ingroup rendering
 * @brief A @ref SHADER uniform variable that
 *        can be set when rendering.
 *
 * **Predefined Uniforms**
 *
 * | Name             | Type      | Description                                |
 * |------------------|-----------|--------------------------------------------|
 * | matWorld         | mat4      | World transformation matrix                |
 * | matView          | mat4      | View transformation matrix                 |
 * | matProj          | mat4      | Projection matrix                          |
 * | matWorldView     | mat4      | matWorld * matView                         |
 * | matViewProj      | mat4      | matWorld * matView                         |
 * | matWorldViewProj | mat4      | matWorld * matView                         |
 * | vecViewDir       | vec3      | Forward vector of the current camera       |
 * | vecViewPos       | vec3      | Position (world) of the current camera     |
 * | vecViewPort      | vec4      | (w,h,1/w,1/h) of the current camera        |
 * | vecColor         | vec3      | @ref MATERIAL.color                        |
 * | vecAttributes    | vec3      | (@ref MATERIAL.roughness, @ref MATERIAL.metallic, @ref MATERIAL.fresnell) |
 * | vecEmission      | vec3      | @ref MATERIAL.emission                     |
 * | texColor         | sampler2D | @ref MATERIAL.colorTexture                 |
 * | texEmission      | sampler2D | @ref MATERIAL.emissionTexture              |
 * | texAttributes    | sampler2D | @ref MATERIAL.attributeTexture             |
 * | *_bmap           | sampler2D | Sampler of a global `BITMAP*` variable     |
 * | *_vec3           | vec3      | Value of a global `VECTOR` variable        |
 * | *_var            | float     | Value of a global `var` variable           |
 */
struct UNIFORM
{
	int location;
	char name[128];
	unsigned int type;
	int sizeInBytes;
	enum FLAGS flags;
	enum SHADERVARIABLE variable;
};

/**
 * @ingroup rendering
 * @brief An OpenGL buffer object
 */
struct BUFFER;

/**
 * @ingroup rendering
 * @brief A 2D texture
 */
struct BITMAP
{
	ACKCONST int width;
	ACKCONST int height;
	enum PIXELFORMAT format;
    ACKNEXT_BITMAP_DETAIL ACKCONST * ACKCONST _detail;
};

/**
 * @ingroup rendering
 * @brief The MATERIAL struct
 */
struct MATERIAL
{
	/**
	 * @brief The shader that is used for rendering this material.
	 *
	 * @remarks If NULL, the default shader will be used.
	 */
	struct SHADER * shader;

	/**
	 * @brief A texture that defines the surface color of an object.
	 *
	 * @remarks If NULL, a white texture will be provided.
	 */
	struct BITMAP * colorTexture;

	/**
	 * @brief A texture that encodes the material attributes on a per pixel
	 *        basis.
	 *
	 * The red color channel contains @ref roughness, the green color channel
	 * the @ref metallic factor and the blue color channel the @ref fresnel
	 * factor. Those texture encoded values are multiplied with the properties
	 * of the material.
	 *
	 * @remarks If NULL, a white texture will be provided.
	 */
	struct BITMAP * attributeTexture;

	/**
	 * @brief A texture encoding the material emission on a per pixel basis.
	 *
	 * @remarks If NULL, a white texture will be provided.
	 */
	struct BITMAP * emissionTexture;

	/**
	 * @brief The color of the object. Reflected light will be tinted with
	 *        this color.
	 *
	 * @remarks The color values provided here will be clamped in the range
	 *          `[0…1]`
	 */
	struct COLOR color; // will be clamped between [0;1]

	/**
	 * @brief The emission of the object. This value will be added after
	 *        the lighting calculation to the final brightness.
	 *
	 * With this effect, bright and glowing objects can be created.
	 */
	struct COLOR emission; // pure additive evil!

	/**
	 * @brief The roughness of the material surface.
	 *
	 * The rougher a material is, the less sharp its reflections are.
	 */
	var roughness;

	/**
	 * @brief The metallic property of the material surface.
	 *
	 * The more metallic a materia is, the less diffuse reflection it will
	 * produce. Also a metallic surface tints the specular reflection instead
	 * of the diffuse reflection.
	 *
	 * @remarks 0% equals poser, 100% is the only true metal form!
	 */
	var metallic;

	/**
	 * @brief The fresnell effect of the surface
	 * @todo write...
	 */
	var fresnell;
};

/**
 * @ingroup rendering
 * @brief A basic graphical component defining a 3D model
 *
 * @todo Add support for bones as well as multiple
 */
struct MESH
{
	struct BUFFER   * vertexBuffer;
	struct BUFFER   * indexBuffer;
	struct MATERIAL * material;
};

/**
 * @ingroup rendering
 * @brief A vertex of a @ref MESH.
 */
struct VERTEX
{
	struct VECTOR position;
	struct VECTOR normal;
	struct VECTOR tangent;
	struct COLOR color;
	struct UVCOORD texcoord0;
	struct UVCOORD texcoord1;
	struct VECTOR4 weights;
} __attribute__((__packed__));

/**
 * @ingroup rendering
 * @brief An index of a @ref MESH.
 */
typedef uint32_t INDEX;

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
_EXPORT_STRUCT(BUFFER)
_EXPORT_STRUCT(VERTEX)
_EXPORT_STRUCT(SHADER)
_EXPORT_STRUCT(UNIFORM)
_EXPORT_STRUCT(UVCOORD)
_EXPORT_STRUCT(VECTOR4)
_EXPORT_STRUCT(MESH)
_EXPORT_STRUCT(BITMAP)
_EXPORT_STRUCT(MATERIAL)
_EXPORT_STRUCT(MATRIX)

typedef enum FLAGS FLAGS;

typedef enum ERROR ERROR;

typedef enum WIDGETTYPE WIDGETTYPE;

#undef _EXPORT_STRUCT

#ifdef __cplusplus
}
#endif

#endif // _ACKNEXT_ACKTYPES_
