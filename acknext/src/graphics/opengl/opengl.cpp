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
	glBindTextureUnit(slot, texture->api().object);
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
			if(buffer->api().type != GL_ARRAY_BUFFER) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer is not a vertex buffer.");
				return;
			}
			if((buffer->api().size % sizeof(VERTEX)) != 0) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer size is not divisible by vertex size.");
				return;
			}
			id = buffer->api().object;
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
			if(buffer->api().type != GL_ELEMENT_ARRAY_BUFFER) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer is not an index buffer.");
				return;
			}
			if((buffer->api().size % sizeof(INDEX)) != 0) {
				engine_seterror(ERR_INVALIDARGUMENT, "Buffer size is not divisible by index size.");
				return;
			}
			id = buffer->api().object;
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
		int mode = 0;
		if(currentIndexBuffer && currentVertexBuffer) {
			mode = 1;
		} else if(currentIndexBuffer) {
			mode = 2;
		} else if(currentVertexBuffer) {
			mode = 3;
		}

		switch(mode) {
			case 0:
				engine_seterror(ERR_INVALIDOPERATION, "There is no vertex or index buffer to be drawn.");
				return;
			case 1:
			case 2:
				if((offset + count) > (currentIndexBuffer->api().size / sizeof(INDEX))) {
					engine_seterror(ERR_INVALIDOPERATION, "offset and count index the index buffer outside of its range.");
					return;
				}
				glDrawElements(
					primitiveType,
					count,
					GL_UNSIGNED_INT,
					(const void *)(sizeof(INDEX) * offset));
				break;
			case 3:
				if((offset + count) > (currentVertexBuffer->api().size / sizeof(VERTEX))) {
					engine_seterror(ERR_INVALIDOPERATION, "offset and count index the vertex buffer outside of its range.");
					return;
				}
				glDrawArrays(
					primitiveType,
					offset,
					count);
				break;
			default: abort();
		}
	}

	void opengl_setShader(SHADER const * shader)
	{
		if((shader != nullptr) && !(shader->flags & LINKED)) {
			engine_seterror(ERR_INVALIDOPERATION, "Trying to render with an unlinked shader!");
			return;
		}

		currentShader = FALLBACK(const_cast<Shader*>(promote<Shader>(shader)), defaultShader);
		glUseProgram(currentShader->api().object);

		currentShader->fGamma = screen_gamma;
		currentShader->vecTime = (VECTOR2){ total_time, time_step };
		currentShader->iDebugMode = opengl_debugMode;
	}

	void opengl_setTexture(int slot, BITMAP const * _texture)
	{
		Bitmap const * texture = FALLBACK(promote<Bitmap>(_texture), defaultWhiteTexture);
		glBindTextureUnit(slot, texture->api().object);
	}

	void opengl_drawMesh(MESH const * mesh)
	{
		if(mesh == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "mesh must not be NULL!");
		}
		opengl_setIndexBuffer(mesh->indexBuffer);
		opengl_setVertexBuffer(mesh->vertexBuffer);

		GLuint count;
		if(mesh->indexBuffer)
			count = mesh->indexBuffer->size / sizeof(INDEX);
		else
			count = mesh->vertexBuffer->size / sizeof(VERTEX);

		GLenum type = mesh->primitiveType;
		if(currentShader->api().flags & TESSELATION)
		{
			type = GL_PATCHES;
			switch(mesh->primitiveType)
			{
				case GL_POINTS:
					glPatchParameteri(GL_PATCH_VERTICES, 1);
					break;
				case GL_LINES:
					glPatchParameteri(GL_PATCH_VERTICES, 2);
					break;
				case GL_TRIANGLES:
					glPatchParameteri(GL_PATCH_VERTICES, 3);
					break;
				case GL_QUADS:
					glPatchParameteri(GL_PATCH_VERTICES, 4);
					break;
				default: abort();
			}
		}
		opengl_draw(
			type,
			0,
			count);
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

		// opengl_setTexture(0, material->colorTexture);
		// opengl_setTexture(1, material->attributeTexture);
		// opengl_setTexture(2, material->emissionTexture);
		// opengl_setTexture(3, material->normalTexture, defaultNormalMap);
	}
}
