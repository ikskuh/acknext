#ifndef SHADERDETAIL_H
#define SHADERDETAIL_H

#include "engine.h"


struct SHADER
{
	GLuint program;
	std::vector<GLuint> shaders;
	bool isLinked;
	std::vector<UNIFORM> uniforms;

	SHADER();
	SHADER(SHADER const &) = delete;
	SHADER(SHADER &&) = delete;
	~SHADER();
};

#endif // SHADERDETAIL_H
