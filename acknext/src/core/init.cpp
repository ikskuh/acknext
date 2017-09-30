#include <engine.hpp>
#include "log.hpp"
#include "config.hpp"
#include "input/inputmanager.hpp"
#include "collision/collisionsystem.hpp"
#include "audio/audiomanager.hpp"
#include "virtfs/resourcemanager.hpp"

#include <chrono>
#include <getopt.h>
#include <physfs.h>
#include <unistd.h>

using std::chrono::steady_clock;
using std::chrono::high_resolution_clock;

// This clock point is used for frame time calculations
static high_resolution_clock::time_point lastFrameTime;

struct engine engine;

bool engine_shutdown_requested = false;

#define SDL_CHECKED(x, y) if((x) < 0) { engine_setsdlerror(); return y; }

// errorhandling.cpp
void engine_setsdlerror();

// graphics-core.cpp
void render_init();
void render_frame();
void render_shutdown();

// scheduler.cpp
void scheduler_init();
void scheduler_shutdown();
void scheduler_update();

ACKNEXT_API_BLOCK
{
	char ** engine_argv = { };
	int engine_argc = 0;

	ENGINESTATS engine_stats;

	EVENT * on_begin_frame;

	int engine_main(void (*main)())
	{
		if(engine_open() == false)
	    {
			fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror_text);
	        return 1;
	    }

		task_start(reinterpret_cast<ENTRYPOINT>(main), nullptr);

	    while(engine_frame())
	    {

	    }

	    engine_close();
		return 0;
	}

	bool engine_open()
	{
		startupTime = std::chrono::steady_clock::now();

		if(engine_config.argv0 == NULL)
		{
			engine_log("ERROR: engine_config.argv0 IS NOT SET. SET THIS TO argv[0] PRIOR TO CALLING engine_open or engine_main!");
			exit(-1);
		}

		engine_log("Begin initalizing engine.");

		// engine_config.load("acknext.cfg");

		if (engine_config.flags & DIAGNOSTIC)
		{
			logfile = fopen("acklog.txt", "w");
			if(logfile == nullptr) {
				engine_log("Failed to open acklog.txt!");
			}
		}

		{
			engine_log("Initialize virtual file system...");

			if(!(engine_config.flags & USE_VFS))
			{
				engine_log("Note: virtual file system is disabled, but required by the engine, so a minimal setup will be done for /builtin.");
			}

			PHYSFS_init(engine_config.argv0);

			if(engine_config.flags & USE_VFS)
			{
				PHYSFS_setSaneConfig (
					engine_config.organization,
					engine_config.application,
					"ARP",    // scan for default archive
					0,        // no cd
					0);       // filesys > pack
			}
			else
			{
				// We could set up something here, but
				// actually, we don't.
			}

			engine_log("app dir:  %s", PHYSFS_getBaseDir());
			engine_log("save dir: %s", PHYSFS_getWriteDir());

			if(engine_config.flags & VFS_USE_CWD)
			{
				char buffer[256];
				PHYSFS_mount(getcwd(buffer,256), NULL, 0);

				engine_log("work dir: %s", buffer);
			}
		}

		engine_log("Initialize builtin resources...");
		ResourceManager::initialize();

		if(!(engine_config.flags & CUSTOM_VIDEO))
		{
			engine_log("Initialize SDL2...");
			SDL_CHECKED(SDL_Init(SDL_INIT_EVERYTHING), false)

			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3), false)
			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3), false)
			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG), false)
			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1), false)

			{ // Create window and initialize SDL
				auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS;
				if(engine_config.flags & FULLSCREEN) {
					if(engine_config.flags & DESKTOP) {
						flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
					} else {
						flags |= SDL_WINDOW_FULLSCREEN;
					}
				}

				engine.window = SDL_CreateWindow(
					engine_config.windowTitle,
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					engine_config.resolution.width, engine_config.resolution.height,
					flags);
				if(engine.window == nullptr)
				{
					engine_setsdlerror();
					return false;
				}

				engine.context = SDL_GL_CreateContext(engine.window);
				if(engine.context == nullptr)
				{
					engine_setsdlerror();
					return false;
				}

				SDL_GetWindowSize(engine.window, &screen_size.width, &screen_size.height);

				// No vsync for debugging
				SDL_GL_SetSwapInterval(0);
			}
		}

		if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) < 0) {
			engine_log("Failed to initialize SDL_image: %s", IMG_GetError());
		}

		on_begin_frame = event_create();

		// engine_log("Initialize collision engine...");
		// collision_init();

		engine_log("Initialize input...");
		InputManager::init();

		engine_log("Initialize renderer...");
		render_init();

		engine_log("Initialize collision system...");
		CollisionSystem::initialize();

		engine_log("Initialize audio system...");
		AudioManager::initialize();

		engine_log("Initialize scheduler...");
		scheduler_init();

		engine_log("Engine ready.");
		engine_log("==========================================================================================");

		lastFrameTime = high_resolution_clock::now();

		// if(compiler_start() == false) {
		// 	return false;
		// }

		engine_shutdown_requested = false;

		return true;
	}

	bool engine_frame()
	{
		auto nextFrameTime = high_resolution_clock::now();
	    // Time Setup
	    {
	        std::chrono::duration<float> timePoint;
	        timePoint = std::chrono::duration_cast<std::chrono::milliseconds>(
	                    nextFrameTime - lastFrameTime);
	        time_step = timePoint.count();

	        timePoint = std::chrono::duration_cast<std::chrono::microseconds>(
	                    steady_clock::now() - startupTime);
	        total_time = timePoint.count();
	    }

		if(!(engine_config.flags & CUSTOM_VIDEO))
		{
			InputManager::beginFrame();

			// Update Frame
			SDL_Event event;
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_QUIT:
						// TODO: Replace with event-call here
						return false;
					case SDL_WINDOWEVENT:
						switch(event.window.event)
						{
							case SDL_WINDOWEVENT_RESIZED:
								engine_resize(event.window.data1, event.window.data2);
								break;
						}
						break;
					case SDL_KEYDOWN:
						InputManager::keyDown(event.key);
						break;
					case SDL_KEYUP:
						InputManager::keyUp(event.key);
						break;
					case SDL_MOUSEBUTTONDOWN:
						InputManager::mouseDown(event.button);
						break;
					case SDL_MOUSEBUTTONUP:
						InputManager::mouseUp(event.button);
						break;
					case SDL_MOUSEMOTION:
						InputManager::mouseMove(event.motion);
						break;
					case SDL_MOUSEWHEEL:
						InputManager::mouseWheel(event.wheel);
						break;
				}
			}
		}

		event_invoke(on_begin_frame, nullptr);

		scheduler_update();

		CollisionSystem::update();

		CollisionSystem::draw();

		render_frame();

	    lastFrameTime = nextFrameTime;
	    total_frames++;
	    return !engine_shutdown_requested;
	}

	void engine_resize(int width, int height)
	{
		screen_size.width = width;
		screen_size.height = height;
		// trigger resize event here
		engine_log("resize to %d×%d", width, height);
	}

	void engine_shutdown()
	{
		engine_shutdown_requested = true;
	}

	void engine_close()
	{
		engine_log("==========================================================================================");
		engine_log("Shutting down scheduler...");
	    scheduler_shutdown();

		engine_log("Shutting down input...");
		InputManager::shutdown();

		engine_log("Shutting down audio system...");
		AudioManager::shutdown();

		engine_log("Shutting down collision system...");
		CollisionSystem::shutdown();

		engine_log("Shutting down renderer...");
		render_shutdown();

		if(!(engine_config.flags & CUSTOM_VIDEO))
		{
			engine_log("Destroy GL context.");
			SDL_GL_DeleteContext(engine.context);

			engine_log("Close window.");
			SDL_DestroyWindow(engine.window);
		}

		engine_log("Shutting down builtin resources...");
		ResourceManager::shutdown();

		engine_log("Shutting down virtual file system...");
		PHYSFS_deinit();

	    engine_log("Engine shutdown complete.");
	}
}
