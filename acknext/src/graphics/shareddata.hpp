#ifndef SHAREDDATA_HPP
#define SHAREDDATA_HPP

#include <engine.hpp>

class Shader;

extern GLuint vao;
extern Shader * defaultShader;
extern Shader * currentShader;
extern BITMAP * defaultWhiteTexture;
extern BITMAP * defaultNormalMap;
extern MESH   * fullscreenQuad;

#endif // SHAREDDATA_HPP