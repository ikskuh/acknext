#ifndef _ACKNEXT_EXT_ACKGUI_H_
#define _ACKNEXT_EXT_ACKGUI_H_

#include <acknext.h>

#define ACKGUI_LAYER 10000

#ifdef __cplusplus

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <imgui.h>
#pragma GCC diagnostic pop

#else
#warning Note that dear imgui only runs with C++, not with C!
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// Initialized and creates a top layer view for dear imgui to draw an
// also initializes dear imgui, so all ImGui::* functions can be called
// afterwards
void ackgui_init();

// returns the view created by ackgui_init
// if no view is created yet, the initialization will be done and
// a view is returned.
VIEW * ackgui_getView();

#ifdef __cplusplus
}
#endif

#endif // _ACKNEXT_EXT_ACKGUI_H_
