#ifndef SHADER_HPP
#define SHADER_HPP

#include <engine.hpp>
#include <vector>

class Shader : public EngineObject<SHADER>
{
public:
	GLuint program;
	std::vector<UNIFORM> uniforms;
	std::vector<GLuint> shaders;
	bool isLinked;
public:
	Shader();
	NOCOPY(Shader);
	~Shader();
};

#endif // SHADER_HPP
