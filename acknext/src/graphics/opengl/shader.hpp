#ifndef SHADER_HPP
#define SHADER_HPP

#include <engine.hpp>
#include <vector>
#include <functional>

template<typename T>
void glProgramUniform(int program, int location, T const & value);

class Shader;

template<typename T>
class UniformProxy
{
friend class Shader;
friend bool shader_link(SHADER * _shader);
private:
	Shader * shader;
	int location;
public:
	UniformProxy() : shader(nullptr), location(-1) { }
	NOCOPY(UniformProxy);
	~UniformProxy() = default;

	inline void operator =(T const & value);
};

class Shader : public EngineObject<SHADER>
{
public:
	GLuint program;
	std::vector<UNIFORM> uniforms;
	std::vector<GLuint> shaders;
#define _UNIFORM(xname, xtype, value, _rtype) UniformProxy<_rtype> xname;
#include "uniformconfig.h"
#undef _UNIFORM
	bool isLinked;
public:
	Shader();
	NOCOPY(Shader);
	~Shader();

	void setUniform(SHADERVAR var, std::function<void(int,int)> const & func);
};


template<typename T>
inline void UniformProxy<T>::operator =(T const & value)
{
	if(location < 0)
		return;
	glProgramUniform(shader->program, location, value);
}

#endif // SHADER_HPP
