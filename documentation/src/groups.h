/**
 * Group Documentation
 */

/**
 * @file
 * @brief Contains documentation
 *
 * @defgroup video Video
 * @brief Functions and variables related to video settings.
 *
 * @defgroup time Time
 * @brief Contains functions and variables related to timing and waiting.
 *
 * @defgroup scheduler Scheduler
 * @brief Contains functions and variables to start and control tasks.
 *
 * The engine provides a cooperative scheduler which allows multiple concurrent
 * tasks to be executed. Each task has a set of its own global variables which
 * will be swapped accordingly.
 *
 * Tasks can be enabled/disabled by using the @ref task_mask and
 * @ref task_enabled. If a bit is set in both variables, the task will be
 * executed.
 *
 * @defgroup scene Levels & Entities
 * @brief In this group is everything related to levels and entities.
 *
 * @defgroup engine Engine
 * @brief Contains functions for engine initialization, logging and error
 *        handling.
 *
 * @defgroup scripting Scripting
 * @brief This module provides basic scripting features.
 *
 * @defgroup gui Graphical User Interface
 * @brief This module provides a 2d graphical user interface.
 *
 * @defgroup rendering Rendering & Shaders
 * @brief This module provides an interface to OpenGL and graphics effects.
 *
 * @defgroup math Math
 * @brief This module provides some basic math for game development
 *
 * @defgroup collision Collision
 * @brief This module provides methods for checking collision and
 *        intersection tests.
 *
 * @defgroup physics Physics
 * @brief This module provides an integration of the ODE physics engine.
 *
 * @defgroup opengl OpenGL API Helpers
 * @brief This module contains functions that help interfacing OpenGL
 *        in combination with engine objects.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
