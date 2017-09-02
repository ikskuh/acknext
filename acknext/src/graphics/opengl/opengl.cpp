#include <engine.hpp>

#include "buffer.hpp"
#include "shader.hpp"
#include "bitmap.hpp"

#define FALLBACK(a, b) ((a) ? (a) : (b))


static Buffer const * currentVertexBuffer;
static Buffer const * currentIndexBuffer;
static Shader * currentShader;

// graphics-core.cpp
extern GLuint vao;
extern Shader * defaultShader;
extern Bitmap * defaultWhiteTexture;
extern Bitmap * defaultNormalMap;

void opengl_setTexture(int slot, BITMAP const * _texture, Bitmap const * _fallback)
{
	Bitmap const * texture = FALLBACK(promote<Bitmap>(_texture), FALLBACK(_fallback, defaultWhiteTexture));
	glBindTextureUnit(slot, texture->id);
}

ACKNEXT_API_BLOCK
{
	int opengl_debugMode = 0;

	bool opengl_wireFrame = false;

	void opengl_setVertexBuffer(BUFFER const * _buffer)
	{
		GLuint id = 0;
		Buffer const * buffer = promote<Buffer>(_buffer);
		if(buffer != nullptr) {
			if(buffer->type != GL_ARRAY_BUFFER) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer is not a vertex buffer.");
				return;
			}
			if((_buffer->size % sizeof(VERTEX)) != 0) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer size is not divisible by vertex size.");
				return;
			}
			id = buffer->id;
		}

		glVertexArrayVertexBuffer(
			vao, 10,
		    id, 0, sizeof(VERTEX));

		currentVertexBuffer = buffer;
	}

	void opengl_setIndexBuffer(BUFFER const * _buffer)
	{
		GLuint id = 0;
		Buffer const * buffer = promote<Buffer>(_buffer);
		if(buffer != nullptr) {
			if(buffer->type != GL_ELEMENT_ARRAY_BUFFER) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer is not an index buffer.");
				return;
			}
			if((_buffer->size % sizeof(INDEX)) != 0) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer size is not divisible by index size.");
				return;
			}
			id = buffer->id;
		}

		glVertexArrayElementBuffer(vao, id);

		currentIndexBuffer = buffer;
	}

	void opengl_setTransform(MATRIX const * matWorld, MATRIX const * matView, MATRIX const * matProj)
	{
		currentShader->matWorld = *matWorld;
		currentShader->matView = *matView;
		currentShader->matProj = *matProj;

		MATRIX mat;

		// TODO: Reimplement this!
		/*
		for(int i = 0; i < cnt; i++)
		{
			UNIFORM const & uni = currentShader->uniforms[i];
			switch(uni.var) {
				case MATWORLD_VAR:
					mat_copy(&mat, matWorld);
					break;
				case MATVIEW_VAR:
					mat_copy(&mat, matView);
					break;
				case MATPROJ_VAR:
					mat_copy(&mat, matProj);
					break;

				case MATWORLDVIEW_VAR: {
					mat_copy(&mat, matWorld);
					mat_mul(&mat, &mat, matView);
					break;
				}
				case MATWORLDVIEWPROJ_VAR: {
					mat_copy(&mat, matWorld);
					mat_mul(&mat, &mat, matView);
					mat_mul(&mat, &mat, matProj);
					break;
				}
				case MATVIEWPROJ_VAR: {
					mat_copy(&mat, matView);
					mat_mul(&mat, &mat, matProj);
					break;
				}

				default:
					// Skip this filthy uniform setting operation!
					continue;
			}

//			engine_log("Matrix %d:\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
//				uni->variable,
//				mat.v[0][0], mat.v[0][1], mat.v[0][2], mat.v[0][3],
//				mat.v[1][0], mat.v[1][1], mat.v[1][2], mat.v[1][3],
//				mat.v[2][0], mat.v[2][1], mat.v[2][2], mat.v[2][3],
//				mat.v[3][0], mat.v[3][1], mat.v[3][2], mat.v[3][3]);

			glProgramUniformMatrix4fv(
				pgm,
				uni.location,
				1,
				GL_FALSE,
				&mat.fields[0][0]);
		}
		*/
	}

	void opengl_draw(
		unsigned int primitiveType,
		unsigned int offset,
		unsigned int count)
	{
		if(currentIndexBuffer == nullptr) {
			engine_seterror(ERR_INVALIDOPERATION, "There is no current index buffer to be drawn.");
			return;
		}
		if((offset + count) > (currentIndexBuffer->api().size / sizeof(INDEX))) {
			engine_seterror(ERR_INVALIDOPERATION, "offset and count index the index buffer outside of its range.");
			return;
		}
		glDrawElements(
			primitiveType,
			count,
			GL_UNSIGNED_INT,
			(const void *)(sizeof(INDEX) * offset));
	}

	void opengl_setShader(SHADER const * shader)
	{
		currentShader = FALLBACK(const_cast<Shader*>(promote<Shader>(shader)), defaultShader);
		glUseProgram(currentShader->program);

		currentShader->fGamma = screen_gamma;
		currentShader->vecTime = (VECTOR2){ total_time, time_step };
		currentShader->iDebugMode = opengl_debugMode;
	}

	void opengl_setTexture(int slot, BITMAP const * _texture)
	{
		Bitmap const * texture = FALLBACK(promote<Bitmap>(_texture), defaultWhiteTexture);
		glBindTextureUnit(slot, texture->id);
	}

	void opengl_setMesh(MESH const * mesh)
	{
		if(mesh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "mesh must not be NULL!");
		}
		opengl_setIndexBuffer(mesh->indexBuffer);
		opengl_setVertexBuffer(mesh->vertexBuffer);
		opengl_setMaterial(mesh->material);
	}

	void opengl_setMaterial(MATERIAL const * material)
	{
		if(material == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "material must not be NULL!");
			return;
		}

		// this fallbacks to the defaultShader if none is set.
		opengl_setShader(material->shader);

		currentShader->vecColor = material->color;
		currentShader->vecAttributes = (VECTOR){material->roughness,material->metallic,material->fresnell};
		currentShader->vecEmission = material->emission;

		opengl_setTexture(0, material->colorTexture);
		opengl_setTexture(1, material->attributeTexture);
		opengl_setTexture(2, material->emissionTexture);
		opengl_setTexture(3, material->normalTexture, defaultNormalMap);
	}
}
