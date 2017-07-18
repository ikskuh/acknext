#include "engine.h"
#include "shader-detail.h"

#include <vector>
#include <unordered_map>
#include <string>

SHADER::SHADER() :
	program(glCreateProgram()),
	shaders(),
	isLinked(false),
	uniforms()
{

}

SHADER::~SHADER()
{
	for(GLuint sh : this->shaders) {
		glDeleteShader(sh);
	}
	glDeleteProgram(this->program);
}

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
		uni->location = i;
		glGetActiveUniform(
			shader->program,
			i,
			sizeof(uni->name),
			nullptr,
			&uni->sizeInBytes,
			&uni->type,
			uni->name);
#define SETTYPE(xname, xtype, value) \
		do { \
			if(strcmp(uni->name, xname) == 0) { \
				if(uni->type != xtype) { \
					engine_seterror(INVALID_OPERATION, "The uniform " xname " is not of the type " #xtype "!"); \
					return false; \
				} \
				uni->variable = value; \
			} \
		} while(false)

		SETTYPE("matWorld", GL_FLOAT_MAT4, MAT_WORLD);
		SETTYPE("matWorldView", GL_FLOAT_MAT4, MAT_WORLDVIEW);
		SETTYPE("matWorldViewProj", GL_FLOAT_MAT4, MAT_WORLDVIEWPROJ);
		SETTYPE("matView", GL_FLOAT_MAT4, MAT_VIEW);
		SETTYPE("matViewProj", GL_FLOAT_MAT4, MAT_VIEWPROJ);
		SETTYPE("matProj", GL_FLOAT_MAT4, MAT_PROJ);

		SETTYPE("vecViewPos", GL_FLOAT_VEC3, VEC_VIEWPOS);
		SETTYPE("vecViewDir", GL_FLOAT_VEC3, VEC_VIEWDIR);
		SETTYPE("vecViewPort", GL_FLOAT_VEC4, VEC_VIEWPORT);

		SETTYPE("vecColor", GL_FLOAT_VEC3, VEC_COLOR);
		SETTYPE("vecEmission", GL_FLOAT_VEC3, VEC_EMISSION);
		SETTYPE("vecAttributes", GL_FLOAT_VEC3, VEC_ATTRIBUTES);

		SETTYPE("texColor", GL_SAMPLER_2D, TEX_COLOR);
		SETTYPE("texAttributes", GL_SAMPLER_2D, TEX_ATTRIBUTES);
		SETTYPE("texEmission", GL_SAMPLER_2D, TEX_EMISSION);

		switch(uni->variable) {
			case TEX_COLOR:
				glProgramUniform1i(shader->program, uni->location, 0);
				break;
			case TEX_ATTRIBUTES:
				glProgramUniform1i(shader->program, uni->location, 1);
				break;
			case TEX_EMISSION:
				glProgramUniform1i(shader->program, uni->location, 2);
				break;
			default:
				break;
		}
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
