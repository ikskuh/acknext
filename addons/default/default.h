#ifndef DEFAULT_H
#define DEFAULT_H

#include <acknext.h>

#ifdef __cplusplus
extern "C" {
#endif

extern var default_movement; //  10 m/s
extern var default_speedup;  // 250 m/s
extern var default_slowdown; //   1 m/s

extern bool default_camera_movement_enabled;

void default_init();

#ifdef __cplusplus
}
#endif

#endif // DEFAULT_H
