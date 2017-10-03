#ifndef DEFAULT_H
#define DEFAULT_H

#include <acknext.h>

#ifdef __cplusplus
extern "C" {
#endif

// Some camera movement variables
extern var default_movement; //  10 m/s
extern var default_speedup;  // 250 m/s
extern var default_slowdown; //   1 m/s

// When this is true, the camera movement will be overridden by
// this extension. You may react to it in your camera control code
// or not
extern bool default_camera_movement_enabled;

// Initialized the default module, attaches events to the F*-keys
// as well as a shorthand to quit the game with alt-escape
void default_init();

#ifdef __cplusplus
}
#endif

#endif // DEFAULT_H
