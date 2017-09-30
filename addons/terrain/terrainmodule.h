#ifndef TERRAINMODULE_H
#define TERRAINMODULE_H

#include <acknext.h>

#ifdef __cplusplus
extern "C" {
#endif

void terrainmodule_init();

float terrain_getheight(MODEL * terrain, float x, float z);

#ifdef __cplusplus
}
#endif

#endif // TERRAINMODULE_H
