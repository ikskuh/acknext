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
    isLinked(false)
{

}

Shader::~Shader()
{
	for(GLuint sh : this->shaders) {
		glDeleteShader(sh);
	}
	glDeleteProgram(this->program);
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

		for(GLuint sh : shader->shaders) {
			glDetachShader(shader->program, sh);
			glDeleteShader(sh);
		}
		shader->shaders.clear();

		if(status != GL_TRUE) {
			engine_seterror(ERR_INVALIDOPERATION, "Failed to link shader!");
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
				&uni->size,
				&uni->type,
				uni->name);

			// TODO: Add all shader variables

	#define SETTYPE(xname, xtype, value) \
			do { \
				if(strcmp(uni->name, xname) == 0) { \
					if(uni->type != xtype) { \
						engine_seterror(ERR_INVALIDOPERATION, "The uniform " xname " is not of the type " #xtype "!"); \
						return false; \
					} \
					uni->var = value; \
				} \
			} while(false)

			SETTYPE("matWorld", GL_FLOAT_MAT4, MATWORLD_VAR);
			SETTYPE("matWorldView", GL_FLOAT_MAT4, MATWORLDVIEW_VAR);
			SETTYPE("matWorldViewProj", GL_FLOAT_MAT4, MATWORLDVIEWPROJ_VAR);
			SETTYPE("matView", GL_FLOAT_MAT4, MATVIEW_VAR);
			SETTYPE("matViewProj", GL_FLOAT_MAT4, MATVIEWPROJ_VAR);
			SETTYPE("matProj", GL_FLOAT_MAT4, MATPROJ_VAR);

			SETTYPE("vecViewPos", GL_FLOAT_VEC3, VECVIEWPOS_VAR);
			SETTYPE("vecViewDir", GL_FLOAT_VEC3, VECVIEWDIR_VAR);
			SETTYPE("vecViewPort", GL_FLOAT_VEC4, VECVIEWPORT_VAR);

			SETTYPE("vecColor", GL_FLOAT_VEC3, VECCOLOR_VAR);
			SETTYPE("vecEmission", GL_FLOAT_VEC3, VECEMISSION_VAR);
			SETTYPE("vecAttributes", GL_FLOAT_VEC3, VECATTRIBUTES_VAR);

			SETTYPE("texColor", GL_SAMPLER_2D, TEXCOLOR_VAR);
			SETTYPE("texAttributes", GL_SAMPLER_2D, TEXATTRIBUTES_VAR);
			SETTYPE("texEmission", GL_SAMPLER_2D, TEXEMISSION_VAR);

			SETTYPE("fGamma", GL_FLOAT, FGAMMA_VAR);

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
				default:
					break;
			}
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
