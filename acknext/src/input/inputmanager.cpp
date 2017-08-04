#include "inputmanager.hpp"
#include "../events/event.hpp"

ACKNEXT_API_BLOCK
{
	POINT mouse_pos = { 0, 0 };
	VECTOR4 mouse_delta = { 0, 0, 0, 0 };

	BUTTONSTATE mouse_left = 0;
	BUTTONSTATE mouse_middle = 0;
	BUTTONSTATE mouse_right = 0;
	BUTTONSTATE mouse_x1 = 0;
	BUTTONSTATE mouse_x2 = 0;

	EVENT * on_mouse_left = nullptr;
	EVENT * on_mouse_middle = nullptr;
	EVENT * on_mouse_right = nullptr;
	EVENT * on_mouse_x1 = nullptr;
	EVENT * on_mouse_x2 = nullptr;
}

void InputManager::init()
{
	on_mouse_left = demote(new Event(false));
	on_mouse_right = demote(new Event(false));
	on_mouse_middle = demote(new Event(false));
	on_mouse_x1 = demote(new Event(false));
	on_mouse_x2 = demote(new Event(false));
}

void InputManager::shutdown()
{
	delete promote<Event>(on_mouse_left);
	delete promote<Event>(on_mouse_middle);
	delete promote<Event>(on_mouse_right);
	delete promote<Event>(on_mouse_x1);
	delete promote<Event>(on_mouse_x2);
}

void InputManager::beginFrame()
{
	mouse_delta = (VECTOR4) { 0, 0, 0, 0 };
}

void InputManager::keyDown(SDL_KeyboardEvent const & ev)
{

}

void InputManager::keyUp(SDL_KeyboardEvent const & ev)
{

}

void InputManager::mouseDown(SDL_MouseButtonEvent const & ev)
{
	switch(ev.button)
	{
		case SDL_BUTTON_LEFT:
			mouse_left = PRESSED;
			event_invoke(on_mouse_left, nullptr);
			break;
		case SDL_BUTTON_RIGHT:
			mouse_right = PRESSED;
			event_invoke(on_mouse_right, nullptr);
			break;
		case SDL_BUTTON_MIDDLE:
			mouse_middle = PRESSED;
			event_invoke(on_mouse_middle, nullptr);
			break;
		case SDL_BUTTON_X1:
			mouse_x1 = PRESSED;
			event_invoke(on_mouse_x1, nullptr);
			break;
		case SDL_BUTTON_X2:
			mouse_x2 = PRESSED;
			event_invoke(on_mouse_x2, nullptr);
			break;
	}
}

void InputManager::mouseUp(SDL_MouseButtonEvent const & ev)
{
	switch(ev.button)
	{
		case SDL_BUTTON_LEFT:
			mouse_left = RELEASED;
			break;
		case SDL_BUTTON_RIGHT:
			mouse_right = RELEASED;
			break;
		case SDL_BUTTON_MIDDLE:
			mouse_middle = RELEASED;
			break;
		case SDL_BUTTON_X1:
			mouse_x1 = RELEASED;
			break;
		case SDL_BUTTON_X2:
			mouse_x2 = RELEASED;
			break;
	}
}

void InputManager::mouseMove(SDL_MouseMotionEvent const & ev)
{
	mouse_pos = (POINT) { ev.x, ev.y };
	mouse_delta.x += ev.xrel;
	mouse_delta.y += ev.yrel;
}

void InputManager::mouseWheel(SDL_MouseWheelEvent const & ev)
{
	mouse_delta.z += ev.x;
	mouse_delta.w += ev.y;
}

