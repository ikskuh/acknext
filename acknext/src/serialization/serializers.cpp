#include <engine.hpp>
#include <acknext/serialization.h>

#include "../extensions/extension.hpp"

static ACKGUID const guidModel =
{{
     0xc4, 0xa6, 0x7f, 0xe0,
	 0x82, 0x74, 0x43, 0x90,
	 0x8e, 0xd6, 0x50, 0x61,
	 0x0b, 0x0a, 0x54, 0xf8
}};
static ACKGUID const guidMaterial =
{{
     0x32, 0x4c, 0x67, 0x80,
	 0x82, 0xb0, 0x47, 0x3d,
	 0xa0, 0xa9, 0xe0, 0xa3,
	 0x4f, 0x6b, 0xee, 0xa2
}};

static ACKGUID const guidBitmap =
{{
     0xb4, 0x0f, 0x64, 0x26,
	 0x2d, 0x6c, 0x44, 0xb3,
	 0xbd, 0x3d, 0x62, 0x9f,
	 0xaa, 0x5a, 0xef, 0xe7
}};

static ACKGUID const guidMesh =
{{
		0xce, 0xb9, 0xe2, 0x22,
		0xc8, 0x03, 0x43, 0xac,
		0x81, 0xa9, 0x14, 0x9a,
		0x32, 0xc5, 0xf3, 0x9b
}};

static ACKGUID const guidShader =
{{
     0xd2, 0x47, 0xfb, 0xc8,
	 0x7a, 0x2a, 0x45, 0x21,
	 0xa2, 0xc6, 0x5a, 0xb5,
	 0x52, 0x2c, 0x28, 0x43
}};


ACKNEXT_API_BLOCK
{
	ACKFUN MODEL * model_read(ACKFILE * file)
	{
		return Extension::load<MODEL>(file);
	}

	void model_write(ACKFILE * file, MODEL const * model)
	{
		Extension::writeHeader(file, TYPE_MODEL, guidModel);

		file_write_uint32(file, model->boneCount);
		file_write_uint32(file, model->meshCount);
		file_write_uint32(file, model->animationCount);

		// bones
		for(int i = 0; i < model->boneCount; i++)
		{
			BONE const & bone = model->bones[i];
			file_write_string(file, bone.name, 64); // TODO: ADJUST/Introduce define
			file_write_uint8(file, bone.parent);
			file_write_matrix(file, bone.transform);
			file_write_matrix(file, bone.bindToBoneTransform);
		}

		// meshes
		for(int i = 0; i < model->meshCount; i++)
		{
			MESH * mesh = model->meshes[i];
			mesh_write(file, mesh);
		}

		// materials
		for(int i = 0; i < model->meshCount; i++)
		{
			MATERIAL const * mtl = model->materials[i];
			mtl_write(file, mtl);
		}
	}

	////////////////////////////////////////////////////////////////////////////

	MESH * mesh_read(ACKFILE * file)
	{
		return Extension::load<MESH>(file);
	}

	void mesh_write(ACKFILE * file, MESH const * mesh)
	{
		Extension::writeHeader(file, TYPE_MESH, guidMesh);

		int indexCount = 0;
		int vertexCount = 0;

		if(mesh->indexBuffer) {
			indexCount = mesh->indexBuffer->size / sizeof(INDEX);
		}
		if(mesh->vertexBuffer) {
			vertexCount = mesh->vertexBuffer->size / sizeof(VERTEX);
		}

		file_write_uint32(file, mesh->primitiveType);
		file_write_uint32(file, indexCount);
		file_write_uint32(file, vertexCount);
		if(mesh->indexBuffer)
		{
			INDEX * indices = (INDEX*)buffer_map(mesh->indexBuffer, GL_READ_ONLY);
			for(int i = 0; i < indexCount; i++) {
				file_write_uint32(file, indices[i]);
			}
			buffer_unmap(mesh->indexBuffer);
		}
		if(mesh->vertexBuffer)
		{
			VERTEX * vertices = (VERTEX*)buffer_map(mesh->vertexBuffer, GL_READ_ONLY);
			for(int i = 0; i < vertexCount; i++) {
				file_write_vector(file, vertices[i].position);
				file_write_vector(file, vertices[i].normal);
				file_write_vector(file, vertices[i].tangent);
				file_write_color(file, vertices[i].color);
				file_write_uv(file, vertices[i].texcoord0);
				file_write_uv(file, vertices[i].texcoord1);
				file_write(file, vertices[i].bones.values, 4);
				file_write(file, vertices[i].boneWeights.values, 4);
			}
			buffer_unmap(mesh->vertexBuffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////

	void mtl_write(ACKFILE * file, MATERIAL const * mtl)
	{
		Extension::writeHeader(file, TYPE_MATERIAL, guidMaterial);

		file_write_color(file, mtl->albedo);
		file_write_color(file, mtl->emission);
		file_write_float(file, mtl->roughness);
		file_write_float(file, mtl->metallic);
		file_write_float(file, mtl->fresnell);

		uint8_t mask = 0;
		if(mtl->albedoTexture)    mask |= 1;
		if(mtl->normalTexture)    mask |= 2;
		if(mtl->attributeTexture) mask |= 4;
		if(mtl->emissionTexture)  mask |= 8;

		file_write_uint8(file, mask);

		if(mtl->albedoTexture)    bmap_write(file, mtl->albedoTexture);
		if(mtl->normalTexture)    bmap_write(file, mtl->normalTexture);
		if(mtl->attributeTexture) bmap_write(file, mtl->attributeTexture);
		if(mtl->emissionTexture)  bmap_write(file, mtl->emissionTexture);
	}

	////////////////////////////////////////////////////////////////////////////

	BITMAP * bmap_read(ACKFILE * file)
	{
		return Extension::load<BITMAP>(file);
	}

	void bmap_write(ACKFILE * file, BITMAP * bitmap)
	{
		if(file == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "file must not be NULL");
			return;
		}
		if(bitmap == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "bitmap must not be NULL");
			return;
		}

		auto const id = bitmap->object;

		file_write(file, "ACKNEXT", 8);
		file_write(file, &ACKNEXT_MAGIC, 4);
		file_write_uint32(file, TYPE_MODEL);
		file_write_guid(file, guidBitmap);

		int width, height, depth;

		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_DEPTH, &depth);

		int rs, gs, bs, as;
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_RED_SIZE, &rs);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_GREEN_SIZE, &gs);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_BLUE_SIZE, &bs);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_ALPHA_SIZE, &as);

		engine_log("%d %d %d", width, height, depth);

		engine_log("%d %d %d %d", rs, gs, bs, as);
	}
}
