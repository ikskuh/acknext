#include "engine.h"

#include <vector>
#include <unordered_map>
#include <string>

struct SHADER
{
	GLuint program;
	std::vector<GLuint> shaders;
	bool isLinked;
	std::vector<UNIFORM> uniforms;

	SHADER() :
	    program(glCreateProgram()),
	    shaders(),
	    isLinked(false),
	    uniforms()
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
	glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		GLint len;
		glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);

		std::vector<GLchar> log(len + 1);
		glGetShaderInfoLog(sh, log.size(), &len, log.data());
		log[len] = 0;

		engine_log("Failed to compile shader: %s", log.data());

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
	glGetProgramiv(shader->program, GL_LINK_STATUS, &status);

	for(GLuint sh : shader->shaders) {
		glDetachShader(shader->program, sh);
		glDeleteShader(sh);
	}
	shader->shaders.clear();

	if(status != GL_TRUE) {
		engine_seterror(INVALID_OPERATION, "Failed to link shader!");
		return false;
	}

	GLint count;
	glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &count);

	shader->uniforms.resize(count);
	for(int i = 0; i < count; i++) {
		UNIFORM * uni = &shader->uniforms[i];
		glGetActiveUniform(
			shader->program,
			i,
			sizeof(uni->name),
			nullptr,
			&uni->sizeInBytes,
			&uni->type,
			uni->name);
	}

	shader->isLinked = true;
	return true;
}

ACKFUN void shader_remove(SHADER * shader)
{
	delete shader;
}

ACKFUN int shader_getUniformCount(SHADER * shader)
{
	if(shader == NULL) {
		engine_seterror(INVALID_ARGUMENT, "shader must not be NULL!");
		return -1;
	}
	if(shader->isLinked == false) {
		engine_seterror(INVALID_OPERATION, "Shader must be linked!");
		return -1;
	}
	return shader->uniforms.size();
}

ACKFUN UNIFORM const * shader_getUniform(SHADER * shader, int index)
{
	if(shader == NULL) {
		engine_seterror(INVALID_ARGUMENT, "shader must not be NULL!");
		return nullptr;
	}
	if(shader->isLinked == false) {
		engine_seterror(INVALID_OPERATION, "Shader must be linked!");
		return nullptr;
	}
	if(index < 0 || index >= int(shader->uniforms.size())) {
		engine_seterror(INVALID_ARGUMENT, "Index of ouf range!");
		return nullptr;
	}
	return &shader->uniforms[index];
}

ACKFUN unsigned int shader_getObject(SHADER * shader)
{
	if(shader == NULL) {
		engine_seterror(INVALID_ARGUMENT, "shader must not be NULL!");
		return 0;
	}
	return shader->program;
}
