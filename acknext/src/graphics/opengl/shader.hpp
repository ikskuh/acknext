#ifndef SHADER_HPP
#define SHADER_HPP

#include <engine.hpp>
#include <vector>
#include <map>
#include <string>
#include <functional>

enum SHADERVAR
{
NULL_VAR = 0,
#define _UNIFORM(xname, xtype, value, _rtype) value,
#include "uniformconfig.h"
#undef _UNIFORM
};

template<typename T>
class UniformProxy;

template<typename T>
void glProgramUniform(int program, UniformProxy<T> & proxy, int location, T const & value);

class Shader;

template<typename T>
class UniformProxy
{
friend class Shader;
friend bool shader_link(SHADER * _shader);
friend void glProgramUniform<T>(int program, UniformProxy<T> & proxy, int location, T const & value);
private:
	Shader * shader;
	UNIFORM * uniform;
public:
	UniformProxy() : shader(nullptr), uniform(nullptr) { }
	NOCOPY(UniformProxy);
	~UniformProxy() = default;

	inline void operator =(T const & value);

	bool present() const { return (this->uniform != nullptr); }
};

class Shader : public EngineObject<SHADER>
{
	template<typename T> friend class UniformProxy;
public:
	std::vector<UNIFORM> uniforms;
	std::vector<GLuint> shaders;
	mutable std::map<std::string, UNIFORM*> uniformsByName;
#define _UNIFORM(xname, xtype, value, _rtype) UniformProxy<_rtype> xname;
#include "uniformconfig.h"
#undef _UNIFORM
	int stub;
public:
	Shader();
	NOCOPY(Shader);
	~Shader();
};


template<typename T>
inline void UniformProxy<T>::operator =(T const & value)
{
	if(this->uniform == nullptr)
		return;
	glProgramUniform(shader->api().object, *this, this->uniform->location, value);
}

#endif // SHADER_HPP
