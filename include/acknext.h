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
 * @ingroup rendering
 * @ingroup engine
 * @brief Swaps the front with the back buffer.
 *
 * @remark This function is useful when the engine
 *         rendering is disabled.
 *
 * @remark This is usually called in @ref engine_frame
 */
ACKFUN void engine_swap();

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
    VIEW const * view,
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
ACKFUN ENTITY * ent_create(char const * source, VECTOR const * position, action (*entmain)());

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
  * @returns A pointer to the created task or null if failure.
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
ACKFUN TASK * task_start(void (*fn)(), void * context);

/**
 * @ingroup scheduler
 * @brief Terminates the given task.
 * @param task A pointer to the task that should be terminated.
 *
 * This function will end the task and release all of its resources.
 */
ACKFUN void task_kill(TASK * task);

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

// TODO: Document new APIs

// SHADER API

/**
 * @ingroup rendering
 * @brief Creates a new shader object
 * @returns The created shader object or NULL on failure.
 */
ACKFUN SHADER * shader_create();

/**
 * @ingroup rendering
 * @brief Adds and compiles a code snippet into the shader.
 *
 * This function internally creates a new shader object and compiles
 * a new OpenGL shader object. On success, this shader object is attached
 * and stored in the @ref SHADER itself.
 *
 * @param shader The Acknext shader object
 * @param
 */
ACKFUN bool shader_addSource(SHADER * shader, enum SHADERTYPE type, const char * source);

/**
 * @ingroup rendering
 * @brief Links the shader after all sources are added.
 *
 * A shader needs to be linked in order to be used for rendering. After linking,
 * no more sources can be added via @ref shader_addSource.
 *
 * @param shader The shader to be linked
 * @returns true on success, else otherwise.
 */
ACKFUN bool shader_link(SHADER * shader);

/**
 * @ingroup rendering
 * @brief Returns the number of active uniforms in a shader.
 * @param shader The shader object
 * @param index  The index of the uniform.
 * @returns Pointer to a @ref UNIFORM object describing the uniform or NULL on
 *          failure.
 */
ACKFUN UNIFORM const * shader_getUniform(SHADER * shader, int index);

/**
 * @ingroup rendering
 * @brief Returns the number of active uniforms in a shader.
 * @param shader The shader object
 * @returns Number of uniforms.
 */
ACKFUN int shader_getUniformCount(SHADER * shader);

/**
 * @ingroup opengl
 * @brief Returns the OpenGL program object for a shader.
 * @param shader The Acknext @ref SHADER object.
 * @returns OpenGL program object
 */
ACKFUN unsigned int shader_getObject(SHADER * shader);

/**
 * @ingroup rendering
 * @brief Destroys a shader object and releases all of its memory.
 */
ACKFUN void shader_remove(SHADER * shader);

// BUFFER API

/**
 * @ingroup rendering
 * @brief Constructs a new buffer object.
 * @param type The type of the buffer object
 * @returns Pointer to the buffer or NULL on failure.
 *
 * Creates a new OpenGL buffer object that can be used to store
 * rendering data.
 *
 * @see BUFFERTYPE
 */
ACKFUN BUFFER * buffer_create(enum BUFFERTYPE type);

/**
 * @ingroup rendering
 * @brief Sets up a buffer object and its data.
 * @param buffer The buffer object that should be initialized.
 * @param size   The size of the buffer in bytes.
 * @param data   A pointer to data of at least `size` bytes or NULL if no initial data.
 *
 * This function initializes a buffer object with a data storage so it
 * can be used for rendering. It also allows to set an inital set of
 * data for the buffer.
 */
ACKFUN void buffer_set(BUFFER * buffer, size_t size, void * data);

/**
 * @ingroup rendering
 * @brief Updates an already initialized buffer with new data.
 * @param buffer The buffer object.
 * @param offset An offset in the buffer. Value is in bytes.
 * @param size   The size of the memory block.
 * @param data   A pointer to the memory block.
 *
 * It copies the contents of the memory referenced via `data` into the buffer.
 */
ACKFUN void buffer_update(BUFFER * buffer, size_t offset, size_t size, void * data);

/**
 * @ingroup opengl
 * @brief Returns the OpenGL object id for the buffer object.
 */
ACKFUN unsigned int buffer_getObject(BUFFER * buffer);

/**
 * @ingroup rendering
 * @brief Destroys a buffer object.
 */
ACKFUN void buffer_remove(BUFFER * buffer);

// Render-API

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setVertexBuffer(BUFFER * buffer);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setIndexBuffer(BUFFER * buffer);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setShader(SHADER * shader);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setMaterial(MATERIAL * material);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setTexture(int slot, BITMAP * texture);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setMesh(MESH * mesh);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_setTransform(MATRIX const * matWorld, MATRIX const * matView, MATRIX const * matProj);

/**
 * @ingroup opengl
 * @todo document
 */
ACKFUN void opengl_draw(
	unsigned int primitiveType,
	unsigned int offset,
	unsigned int count);

// Bitmap API

/**
 * @ingroup rendering
 * @brief Creates a new bitmap texture and returns a pointer to it.
 * @remarks This texture is not yet initialized and must be set up with
 *          @ref bmap_set.
 * @see bmap_set
 */
ACKFUN BITMAP * bmap_create();

/**
 * @ingroup rendering
 * @brief Creates a new bitmap.
 *
 * @param width  The width of the bitmap in pixels.
 * @param height The height of the bitmap in pixels.
 * @param format The pixel format this bitmap uses.
 *
 * @see PIXELFORMAT
 * @see bmap_create
 * @see bmap_set
 */
ACKFUN BITMAP * bmap_createblack(int width, int height, enum PIXELFORMAT format);

/**
 * @ingroup rendering
 * @brief Loads a bitmap from a file.
 *
 * @param fileName The name of the file that should be loaded
 * @returns Pointer to the loaded bitmap or NULL on failure.
 */
ACKFUN BITMAP * bmap_load(char const * fileName);

/**
 * @ingroup rendering
 * @brief Sets the contents of the bitmap.
 *
 * @param bitmap The bitmap which should be set up.
 * @param width  The width of the image data in pixels.
 * @param height The height of the image data in pixels.
 * @param format The format of the bitmap.
 * @param data   A pointer to the initial image data or NULL if default
 *
 * This function initializes and prefills a bitmap texture. It sets the
 * size, format and optionally the initial data.
 *
 * @todo document data depending on format
 */
ACKFUN void bmap_set(BITMAP * bitmap, int width, int height, enum PIXELFORMAT format, void const * data);

/**
 * @ingroup rendering
 * @brief Destroys a previously created or loaded bitmap.
 *
 * @see bmap_create
 * @see bmap_createblack
 * @see bmap_load
 */
ACKFUN void bmap_remove(BITMAP * bitmap);

// Material API

/**
 * @ingroup rendering
 * @brief Constructs a new material
 */
ACKFUN MATERIAL * mtl_create();

/**
 * @ingroup rendering
 * @brief Destroys a previously constructed material.
 * @see mtl_create
 */
ACKFUN void mtl_remove(MATERIAL * mtl);

// Value API

/**
 * @ingroup math
 * @todo document
 */
ACKFUN void mat_id(MATRIX * mat);

/**
 * @ingroup math
 * @todo document
 */
ACKFUN void mat_mul(MATRIX * mat, MATRIX const * lhs, MATRIX const * rhs);

/**
 * @ingroup math
 * @todo document
 */
ACKFUN void mat_copy(MATRIX * dst, MATRIX const * src);


/**
 * @ingroup math
 * @todo document
 */
ACKFUN void vec_zero(VECTOR * dst);

/**
 * @ingroup math
 * @todo document
 */
ACKFUN void vec_set(VECTOR * dst, VECTOR const * src);

/**
 * @ingroup math
 * @todo document
 */
ACKFUN void quat_id(QUATERNION * dst);

/**
 * @ingroup math
 * @todo document
 */
ACKFUN void quat_set(QUATERNION * dst, QUATERNION const * src);

////////////////////////////////////////////////////////////////////////////////

#ifndef ACKNEXT_NO_PRIMITIVE_CTORS

/**
 * @ingroup math
 * @brief Constructs a vector
 * @param x
 * @param y
 * @param z
 * @return
 */
ACKFUN VECTOR const * vector(var x, var y, var z);

/**
 * @ingroup math
 * @brief Constructions a point
 * @param x
 * @param y
 * @return
 */
static inline POINT point(int x, int y)
{
    return (POINT){ x, y };
}

/**
 * @ingroup math
 * @brief Constructs a size
 * @param width
 * @param height
 * @return
 */
static inline SIZE size(int width, int height)
{
    return (SIZE){ width, height };
}

/**
 * @ingroup math
 * @brief Returns a quaternion based on euler angles
 * @param pan  The rotation around the y-axis
 * @param tilt The rotation around the y-axis
 * @param roll The rotation around the x-axis
 * @return Quaternion defining the euler rotation.
 */
ACKFUN QUATERNION const * euler(var pan, var tilt, var roll);

/**
 * @ingroup rendering
 * @brief Constructs a color from hexadecimal notation.
 * @param hex Hexadezimal notation in the format `0xAARRGGBB`
 * @remark If alpha is not "defined" in the hex code, it will be 0!
 * @return COLOR structure
 */
static inline COLOR hexcolor(uint32_t hex)
{
	return (COLOR) {
		(uint8_t)(hex >> 16) / 255.0f,
		(uint8_t)(hex >>  8) / 255.0f,
		(uint8_t)(hex >>  0) / 255.0f,
		(uint8_t)(hex >> 24) / 255.0f,
	};
}

#endif // ACKNEXT_NO_PRIMITIVE_CTORS

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
