#include <engine.hpp>
#include "shader.hpp"

#define FUNC(_type, call) template<> void glProgramUniform<_type>(int p, int l, _type const & v) { \
	call; \
} \

FUNC(bool, glProgramUniform1i(p, l, v ? GL_TRUE : GL_FALSE))
FUNC(int, glProgramUniform1i(p, l, v))
FUNC(float, glProgramUniform1f(p, l, v))
FUNC(VECTOR2, glProgramUniform2f(p, l, v.x, v.y))
FUNC(VECTOR, glProgramUniform3f(p, l, v.x, v.y, v.z))
FUNC(VECTOR4, glProgramUniform4f(p, l, v.x, v.y, v.z, v.w))
FUNC(MATRIX,glProgramUniformMatrix4fv(p, l, 1, GL_FALSE, &v.fields[0][0]))
FUNC(COLOR, glProgramUniform4f(p, l, v.red, v.green, v.blue, v.alpha))
