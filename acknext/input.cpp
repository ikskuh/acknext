#include "engine.h"





void input_update()
{
    int numkeys;
    uint8_t const * keymap = SDL_GetKeyboardState(&numkeys);

#define _ACKKEY(name, key, scan) key_##name = (keymap[scan] > 0) ? 1 : 0;
#include <ackkeys.h>
#undef _ACKKEY

}


void input_callback(int key)
{
    switch(key)
    {
#define _ACKKEY(name, key, sym) case key: if(on_##name != nullptr) on_##name(); break;
#include <ackkeys.h>
#undef _ACKKEY
    }
}
