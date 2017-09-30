#include "shader.hpp"

Shader::Shader() :
    EngineObject<SHADER>(),
    uniforms(), shaders(),
#define _UNIFORM(xname, xtype, value, _rtype) xname(),
#include "uniformconfig.h"
#undef _UNIFORM
    stub(42) // required for termination
{
	api().object = glCreateProgram();
#define _UNIFORM(xname, xtype, value, _rtype) this->xname.shader = this;
#include "uniformconfig.h"
#undef _UNIFORM
}

Shader::~Shader()
{
	for(GLuint sh : this->shaders) {
		glDeleteShader(sh);
	}
	glDeleteProgram(api().object);
}

ACKNEXT_API_BLOCK
{
	SHADER * shader_create()
	{
		return demote(new Shader());
	}

	bool shader_addSource(SHADER * _shader, GLenum type, const char * source)
	{
		Shader * shader = promote<Shader>(_shader);
		if(shader == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return false;
		}
		if(shader->api().flags & LINKED) {
			engine_seterror(ERR_INVALIDOPERATION, "Shader is already linked!");
			return false;
		}

		GLuint sh = glCreateShader(type);
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
		glAttachShader(shader->api().object, sh);

		if(type == TESSCTRLSHADER || type == TESSEVALSHADER) {
			shader->api().flags |= TESSELATION;
		}

		return true;
	}

	bool shader_addFileSource(SHADER * shader, GLenum type, const char * fileName)
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
		if(shader->api().flags & LINKED) {
			engine_seterror(ERR_INVALIDOPERATION, "Shader is already linked!");
			return false;
		}
		GLint status;
		GLuint const program = shader->api().object;

		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &status);

		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		std::vector<GLchar> log(len + 1);
		glGetProgramInfoLog(shader->api().object, log.size(), &len, log.data());
		log[len] = 0;

		for(GLuint sh : shader->shaders) {
			glDetachShader(program, sh);
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

		_shader->textureSlotCount = 0;
		{
			GLint count;
			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

			shader->uniforms.resize(count);
			for(int i = 0; i < count; i++) {
				UNIFORM * uni = &shader->uniforms[i];
				glGetActiveUniform(
					program,
					i,
					sizeof(uni->name),
					nullptr,
					&uni->size,
					&uni->type,
					uni->name);
				uni->index = i;
				uni->block = -1; // No block by default
				uni->location = glGetUniformLocation(program, uni->name);

				shader->uniformsByName.emplace(uni->name, uni);

	#define _UNIFORM(xname, xtype, value, _rtype) \
				do { \
					if(strcmp(uni->name, #xname) == 0) { \
						if(uni->type != xtype) { \
							engine_seterror(ERR_INVALIDOPERATION, "The uniform " #xname " is not of the type " #xtype "!"); \
							return false; \
						} \
						uni->var = value; \
						shader->xname.uniform = uni; \
					} \
				} while(false);
	#include "uniformconfig.h"
	#undef _UNIFORM

				if(Property::isSampler(uni->type))
				{
					// preinitialize uniforms with correct slut
					uni->textureSlot = _shader->textureSlotCount++;
					glProgramUniform1i(program, uni->location, uni->textureSlot);
				}
				else
				{
					uni->textureSlot = -1;
				}
			}
		}

		{
			int count;
			glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count);
			for(int i = 0; i < count; i++)
			{
				int len;
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &len);
				int locations[len];
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, locations);
				for(int j = 0; j < len; j++) {
					// Now associate each uniform with its uniform block
					shader->uniforms[locations[j]].block = i;
				}
			}
		}

		shader->api().flags |= LINKED;
		return true;
	}


	ACKFUN void shader_logInfo(SHADER const * _shader)
	{
		Shader const * shader = promote<Shader>(_shader);
		if(shader == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return;
		}
		if(!(shader->api().flags & LINKED)) {
			engine_seterror(ERR_INVALIDOPERATION, "Shader is not linked!");
			return;
		}
		engine_log("Shader %d:", _shader->object);
		GLuint const program = shader->api().object;
		{
			int count;
			glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count);
			for(int i = 0; i < count; i++)
			{
				int len;
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &len);

				char name[len + 1];
				glGetActiveUniformBlockName(program, i, len + 1, NULL, name);

				engine_log("\tUniform Buffer Block %d: %s", i, name);
			}
		}

		for(UNIFORM const & uni : shader->uniforms)
		{
			char const * variable = "";
			switch(uni.var) {
#define _UNIFORM(xname, xtype, value, _rtype) \
				case value: variable = "(" #value ")"; break;
#include "uniformconfig.h"
#undef _UNIFORM
			}

			engine_log(
				"\tUniform: [%d:%2d] %s%s, size=%d, slot=%d",
				uni.block,
				uni.location,
				uni.name,
				variable,
				uni.size,
				uni.textureSlot);
		}
	}

	UNIFORM const * shader_getUniform(SHADER const * shader, int index)
	{
		Shader const * sh = promote<Shader>(shader);
		if(sh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return nullptr;
		}
		if(!(sh->api().flags & LINKED)) {
			engine_seterror(ERR_INVALIDOPERATION, "shader must be linked!");
			return nullptr;
		}
		if(index < 0 || index >= int(sh->uniforms.size())) {
			engine_seterror(ERR_OUTOFBOUNDS, "index out of bounds!");
			return nullptr;
		}
		return &sh->uniforms[index];
	}

	int shader_getUniformCount(SHADER const * shader)
	{
		Shader const * sh = promote<Shader>(shader);
		if(sh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return -1;
		}
		if(!(sh->api().flags & LINKED)) {
			engine_seterror(ERR_INVALIDOPERATION, "shader must be linked!");
			return -1;
		}
		return int(sh->uniforms.size());
	}

	UNIFORM const * shader_getUniformByName(const SHADER *shader, const char *name)
	{
		const int cnt = shader_getUniformCount(shader);
		if(cnt < 0) {
			return nullptr;
		}
		return promote<Shader>(shader)->uniformsByName[name];
		for(int i = 0; i < cnt; i++)
		{
			UNIFORM const * uni = shader_getUniform(shader, i);
			if(strcmp(uni->name, name) == 0) {
				return uni;
			}
		}
		return nullptr;
	}

	void shader_setUniforms(SHADER * shader, void const * source, bool override)
	{
		Shader * sh = promote<Shader>(shader);
		if(sh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "shader must not be NULL!");
			return;
		}
		if(!(sh->api().flags & LINKED)) {
			engine_seterror(ERR_INVALIDOPERATION, "shader must be linked!");
			return;
		}
		Dummy const * dummy = promote<Dummy>(reinterpret_cast<DUMMY const*>(source));
		if(dummy == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "source must be an engine object!");
			return;
		}
		const GLuint pgm = shader->object;
		for(auto const & prop : dummy->properties)
		{
			UNIFORM const * uni = sh->uniformsByName[prop.first];
			if(uni == nullptr) {
				continue;
			}
			Property const & p = prop.second;
			if(uni->type != p.type) {
				engine_log("Warning: %s does not match the shader specification!", uni->name);
				continue;
			}
			const int loc = uni->location;

			if(p.isSampler())
			{
				if(p.data.texture || override) {
					opengl_setTexture(uni->textureSlot, p.data.texture);
				}
			}
			else
			{
				switch(p.type)
				{
					case GL_INT:
						glProgramUniform1i(pgm, loc, p.data.ints[0]);
						break;
					case GL_INT_VEC2:
						glProgramUniform2i(pgm, loc, p.data.ints[0], p.data.ints[1]);
						break;
					case GL_INT_VEC3:
						glProgramUniform3i(pgm, loc, p.data.ints[0], p.data.ints[1], p.data.ints[2]);
						break;
					case GL_INT_VEC4:
						glProgramUniform4i(pgm, loc, p.data.ints[0], p.data.ints[1], p.data.ints[2], p.data.ints[3]);
						break;
					case GL_FLOAT:
						glProgramUniform1f(pgm, loc, p.data.floats[0]);
						break;
					case GL_FLOAT_VEC2:
						glProgramUniform2f(pgm, loc, p.data.floats[0], p.data.floats[1]);
						break;
					case GL_FLOAT_VEC3:
						glProgramUniform3f(pgm, loc, p.data.floats[0], p.data.floats[1], p.data.floats[2]);
						break;
					case GL_FLOAT_VEC4:
						glProgramUniform4f(pgm, loc, p.data.floats[0], p.data.floats[1], p.data.floats[2], p.data.floats[3]);
						break;
					// TODO: Add more, also add support for samplers!
					default:
						engine_log("Warning: %s has an unsupported property type!", uni->name);
						continue;
				}
			}
		}
	}

	void shader_remove(SHADER * shader)
	{
		Shader * sh = promote<Shader>(shader);
		if(sh) {
			delete sh;
		}
	}
}
