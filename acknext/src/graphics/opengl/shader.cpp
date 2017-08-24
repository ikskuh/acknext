#include "shader.hpp"

static GLenum getShaderType(SHADERTYPE type)
{
	switch(type) {
		case VERTEXSHADER: return GL_VERTEX_SHADER;
		case FRAGMENTSHADER: return GL_FRAGMENT_SHADER;
		case GEOMETRYSHADER: return GL_GEOMETRY_SHADER;
		case TESSCTRLSHADER: return GL_TESS_CONTROL_SHADER;
		case TESSEVALSHADER: return GL_TESS_EVALUATION_SHADER;
		default:
			engine_seterror(ERR_INVALIDARGUMENT, "%d is not a valid shader type!", type);
			return GL_INVALID_ENUM;
	}
}

Shader::Shader() :
    EngineObject<SHADER>(),
    program(glCreateProgram()),
    uniforms(), shaders(),
#define _UNIFORM(xname, xtype, value, _rtype) xname(),
#include "uniformconfig.h"
#undef _UNIFORM
    isLinked(false)
{
#define _UNIFORM(xname, xtype, value, _rtype) this->xname.shader = this;
#include "uniformconfig.h"
#undef _UNIFORM
}

Shader::~Shader()
{
	for(GLuint sh : this->shaders) {
		glDeleteShader(sh);
	}
	glDeleteProgram(this->program);
}

void Shader::setUniform(SHADERVAR var, std::function<void(int,int)> const & func)
{
	for(UNIFORM & u : this->uniforms)
	{
		if(u.var != var) {
			continue;
		}
		func(this->program, u.location);
	}
}

ACKNEXT_API_BLOCK
{
	SHADER * shader_create()
	{
		return demote(new Shader());
	}

	bool shader_addSource(SHADER * _shader, SHADERTYPE type, const char * source)
	{
		Shader * shader = promote<Shader>(_shader);
		if(shader == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return false;
		}
		if(shader->isLinked) {
			engine_seterror(ERR_INVALIDOPERATION, "Shader is already linked!");
			return false;
		}

		GLuint sh = glCreateShader(getShaderType(type));
		if(sh == 0) {
			return false;
		}
		glShaderSource(sh, 1, &source, nullptr);
		glCompileShader(sh);

		GLint status;
		glGetShaderiv(sh, GL_COMPILE_STATUS, &status);

		GLint len;
		glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);

		std::vector<GLchar> log(len + 1);
		glGetShaderInfoLog(sh, log.size(), &len, log.data());
		log[len] = 0;

		if(status != GL_TRUE) {
			engine_log("Failed to compile shader: %s", log.data());
			glDeleteShader(sh);
			return false;
		}

		if(len > 0) {
			engine_log("shader log: %s", log.data());
		}

		shader->shaders.push_back(sh);
		glAttachShader(shader->program, sh);

		return true;
	}

	bool shader_addFileSource(SHADER * shader, SHADERTYPE type, const char * fileName)
	{
		BLOB * blob = blob_load(fileName);
		if(!blob) {
			return false;
		}
		bool result = shader_addSource(shader, type, (char*)blob->data);
		blob_remove(blob);
		return result;
	}

	bool shader_link(SHADER * _shader)
	{
		Shader * shader = promote<Shader>(_shader);
		if(shader == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return false;
		}
		if(shader->isLinked) {
			engine_seterror(ERR_INVALIDOPERATION, "Shader is already linked!");
			return false;
		}
		glLinkProgram(shader->program);
		GLint status;
		glGetProgramiv(shader->program, GL_LINK_STATUS, &status);

		GLint len;
		glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &len);

		std::vector<GLchar> log(len + 1);
		glGetProgramInfoLog(shader->program, log.size(), &len, log.data());
		log[len] = 0;

		for(GLuint sh : shader->shaders) {
			glDetachShader(shader->program, sh);
			glDeleteShader(sh);
		}
		shader->shaders.clear();

		if(status != GL_TRUE) {
			engine_log("Failed to link shader: %s", log.data());
			engine_seterror(ERR_INVALIDOPERATION, "Failed to link shader!");
			return false;
		}
		if(len > 0) {
			engine_log("link log: %s", log.data());
		}

		{
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
					&uni->size,
					&uni->type,
					uni->name);
				uni->index = i;
				uni->block = -1; // No block by default
				uni->location = glGetUniformLocation(shader->program, uni->name);

	#define _UNIFORM(xname, xtype, value, _rtype) \
				do { \
					if(strcmp(uni->name, #xname) == 0) { \
						if(uni->type != xtype) { \
							engine_seterror(ERR_INVALIDOPERATION, "The uniform " #xname " is not of the type " #xtype "!"); \
							return false; \
						} \
						uni->var = value; \
						shader->xname.location = uni->location; \
					} \
				} while(false);
	#include "uniformconfig.h"
	#undef _UNIFORM

				switch(uni->var) {
					case TEXCOLOR_VAR:
						glProgramUniform1i(shader->program, uni->location, 0);
						break;
					case TEXATTRIBUTES_VAR:
						glProgramUniform1i(shader->program, uni->location, 1);
						break;
					case TEXEMISSION_VAR:
						glProgramUniform1i(shader->program, uni->location, 2);
						break;
					case TEXNORMALMAP_VAR:
						glProgramUniform1i(shader->program, uni->location, 3);
						break;
					default:
						break;
				}
			}
		}

		{
			int count;
			glGetProgramiv(shader->program, GL_ACTIVE_UNIFORM_BLOCKS, &count);
			for(int i = 0; i < count; i++)
			{
				int len;
				glGetActiveUniformBlockiv(shader->program, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &len);

				char name[len + 1];
				glGetActiveUniformBlockName(shader->program, i, len + 1, NULL, name);

				engine_log("Uniform Buffer Block %d: %s", i, name);

				glGetActiveUniformBlockiv(shader->program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &len);
				int locations[len];
				glGetActiveUniformBlockiv(shader->program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, locations);
				for(int j = 0; j < len; j++) {
					// Now associate each uniform with its uniform block
					shader->uniforms[locations[j]].block = i;
				}
			}
		}

		for(UNIFORM const & uni : shader->uniforms)
		{
			char const * variable = "Unknown";
			switch(uni.var) {
#define _UNIFORM(xname, xtype, value, _rtype) \
				case value: variable = #value; break;
#include "uniformconfig.h"
#undef _UNIFORM
			}

			engine_log("Uniform: [%d:%2d] %s \t→ %s, %d", uni.block, uni.location, uni.name, variable, uni.size); \
		}

		shader->isLinked = true;
		return true;
	}

	UNIFORM const * shader_getUniform(SHADER * shader, int index)
	{
		Shader * sh = promote<Shader>(shader);
		if(sh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return nullptr;
		}
		if(sh->isLinked == false) {
			engine_seterror(ERR_INVALIDOPERATION, "shader must be linked!");
			return nullptr;
		}
		if(index < 0 || index >= int(sh->uniforms.size())) {
			engine_seterror(ERR_OUTOFBOUNDS, "index out of bounds!");
			return nullptr;
		}
		return &sh->uniforms[index];
	}

	int shader_getUniformCount(SHADER * shader)
	{
		Shader * sh = promote<Shader>(shader);
		if(sh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return -1;
		}
		if(sh->isLinked == false) {
			engine_seterror(ERR_INVALIDOPERATION, "shader must be linked!");
			return -1;
		}
		return int(sh->uniforms.size());
	}

	GLDATA shader_getObject(SHADER * shader)
	{
		Shader * sh = promote<Shader>(shader);
		if(sh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return 0;
		}
		return sh->program;
	}

	void shader_remove(SHADER * shader)
	{
		Shader * sh = promote<Shader>(shader);
		if(sh) {
			delete sh;
		}
	}
}
