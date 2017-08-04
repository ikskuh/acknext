#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <engine.hpp>

class InputManager
{
public:
	InputManager() = delete;

	static void init();
	static void shutdown();

	static void beginFrame();

	static void keyDown(SDL_KeyboardEvent const & ev);
	static void keyUp(SDL_KeyboardEvent const & ev);

	static void mouseDown(SDL_MouseButtonEvent const & ev);
	static void mouseUp(SDL_MouseButtonEvent const & ev);
	static void mouseMove(SDL_MouseMotionEvent const & ev);
	static void mouseWheel(SDL_MouseWheelEvent const & ev);
};

#endif // INPUTMANAGER_HPP
