#include "engine.h"

#include <vector>
#include <unordered_map>
#include <string>

struct SHADER
{
	GLuint program;
	std::vector<GLuint> shaders;
	bool isLinked;
	std::unordered_map<std::string, GLint> uniforms;

	SHADER() : program(glCreateProgram()), shaders(), isLinked(false)
	{

	}

	~SHADER()
	{
		for(GLuint sh : this->shaders) {
			glDeleteShader(sh);
		}
		glDeleteProgram(this->program);
	}
};

static GLenum transpose(enum SHADERTYPE type)
{
	switch(type) {
		case VERTEX_SHADER: return GL_VERTEX_SHADER;
		case FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
		default:
			engine_seterror(INVALID_ARGUMENT, "%d is not a valid shader type!", type);
			return GL_INVALID_ENUM;
	}
}

ACKFUN SHADER * shader_create()
{
	return new SHADER();
}

ACKFUN bool shader_addSource(SHADER * shader, enum SHADERTYPE type, const char * source)
{
	if(shader == NULL) {
		engine_seterror(INVALID_ARGUMENT, "shader must not be NULL!");
		return false;
	}
	if(shader->isLinked) {
		engine_seterror(INVALID_OPERATION, "Shader is already linked!");
		return false;
	}
	GLuint sh = glCreateShader(transpose(type));
	if(sh == 0) {
		return false;
	}
	glShaderSource(sh, 1, &source, nullptr);
	glCompileShader(sh);
	GLint status;
	glGetShaderiv(sh, 1, &status);
	if(status != GL_TRUE) {
		glDeleteShader(sh);
		return false;
	}

	shader->shaders.push_back(sh);
	glAttachShader(shader->program, sh);

	return true;
}

ACKFUN bool shader_link(SHADER * shader)
{
	if(shader == NULL) { engine_seterror(INVALID_ARGUMENT, "shader must not be NULL!"); return false; }
	if(shader->isLinked) {
		engine_seterror(INVALID_OPERATION, "Shader is already linked!");
		return false;
	}
	glLinkProgram(shader->program);
	GLint status;
	glGetProgramiv(shader->program, 1, &status);

	for(GLuint sh : shader->shaders) {
		glDetachShader(shader->program, sh);
		glDeleteShader(sh);
	}
	shader->shaders.clear();

	return (status == GL_TRUE);
}

ACKFUN void shader_remove(SHADER * shader)
{
	delete shader;
}

ACKFUN bool shader_use(SHADER * shader)
{
	if(shader == NULL) {
		engine_seterror(INVALID_ARGUMENT, "shader must not be NULL!");
		return false;
	}
	if(shader->isLinked == false) {
		engine_seterror(INVALID_OPERATION, "Shader must be linked!");
		return false;
	}
	glUseProgram(shader->program);
}

ACKFUN UNIFORM * shader_getUniform(SHADER * shader, char const * name)
{

}

ACKFUN unsigned int shader_getObject(SHADER * shader);
