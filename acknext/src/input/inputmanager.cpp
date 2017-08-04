#include "inputmanager.hpp"
#include "../events/event.hpp"

ACKNEXT_API_BLOCK
{
#define _ACKNEXT_KEYDEF(_name,_scancode) BUTTONSTATE key_##_name = RELEASED;
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF

#define _ACKNEXT_KEYDEF(_name,_scancode) EVENT * on_##_name = nullptr;
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF

	BUTTONSTATE key_any = RELEASED;
	EVENT * on_anykey = nullptr;

	int key_lastpressed = 0;

	POINT mouse_pos = { 0, 0 };
	VECTOR4 mickey = { 0, 0, 0, 0 };

	BUTTONSTATE mouse_left = RELEASED;
	BUTTONSTATE mouse_middle = RELEASED;
	BUTTONSTATE mouse_right = RELEASED;
	BUTTONSTATE mouse_x1 = RELEASED;
	BUTTONSTATE mouse_x2 = RELEASED;

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

#define _ACKNEXT_KEYDEF(_name,_scancode) on_##_name = demote(new Event(false));
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF

	on_anykey = demote(new Event(false));
}

void InputManager::shutdown()
{
	delete promote<Event>(on_mouse_left);
	delete promote<Event>(on_mouse_middle);
	delete promote<Event>(on_mouse_right);
	delete promote<Event>(on_mouse_x1);
	delete promote<Event>(on_mouse_x2);

#define _ACKNEXT_KEYDEF(_name,_scancode) delete promote<Event>(on_##_name);
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF

	delete promote<Event>(on_anykey);
}

void InputManager::beginFrame()
{
	mickey = (VECTOR4) { 0, 0, 0, 0 };
}

static void update_anykey()
{
	// This hurts a bit, but is also kinda cool
	key_any = (0
#define _ACKNEXT_KEYDEF(_name,_scancode) || key_##_name
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF
		) ? PRESSED : RELEASED;
}

void InputManager::keyDown(SDL_KeyboardEvent const & ev)
{
	switch(ev.keysym.scancode)
	{
#define _ACKNEXT_KEYDEF(_name,_scancode) \
		case _scancode: \
			key_##_name = PRESSED; \
			event_invoke(on_##_name, nullptr); \
			break;
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF
		case SDL_SCANCODE_UNKNOWN:
			engine_log("You pressed a key that isn't known to SDL! Bad boy!");
			return;
		case SDL_NUM_SCANCODES: abort(); // Just hard-die here
	}
	key_lastpressed = ev.keysym.scancode;
	update_anykey();
	event_invoke(on_anykey, nullptr);
}

void InputManager::keyUp(SDL_KeyboardEvent const & ev)
{
	switch(ev.keysym.scancode)
	{
#define _ACKNEXT_KEYDEF(_name,_scancode) \
		case _scancode: \
			key_##_name = RELEASED; \
			break;
#include <acknext/keyboard-config.h>
#undef _ACKNEXT_KEYDEF
		case SDL_SCANCODE_UNKNOWN:
			engine_log("You pressed a key that isn't known to SDL! Bad boy!");
			return;
		case SDL_NUM_SCANCODES: abort(); // Just hard-die here
	}
	update_anykey();
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
	mickey.x += ev.xrel;
	mickey.y += ev.yrel;
}

void InputManager::mouseWheel(SDL_MouseWheelEvent const & ev)
{
	mickey.z += ev.x;
	mickey.w += ev.y;
}

