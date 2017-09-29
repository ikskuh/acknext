#include <engine.hpp>
#include <assert.h>
#include <acknext/extension.h>
#include <acknext/serialization.h>
#include <acknext/acff.h>

#include "../graphics/core/glenum-translator.hpp"
#include "../extensions/extension.hpp"

ACKNEXT_API_BLOCK
{
	ACKFUN MODEL * model_read(ACKFILE * file)
	{
		return Extension::load<MODEL>(file);
	}

	void model_write(ACKFILE * file, MODEL const * model)
	{
		Extension::writeHeader(file, TYPE_MODEL, acff_guidModel);

		file_write_uint32(file, model->boneCount);
		file_write_uint32(file, model->meshCount);
		file_write_uint32(file, model->animationCount);
		file_write_uint32(file, model->minimumLOD);

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
			mesh_write(file, model->meshes[i]);
		}

		// materials
		for(int i = 0; i < model->meshCount; i++)
		{
			mtl_write(file, model->materials[i]);
		}

		for(int i = 0; i < model->animationCount; i++)
		{
			ANIMATION const * anim = model->animations[i];
			file_write_string(file, anim->name, 0);
			file_write_float(file, anim->duration);
			file_write_uint32(file, anim->flags);
			file_write_uint32(file, anim->channelCount);
			for(int i = 0; i < anim->channelCount; i++)
			{
				CHANNEL const * chan = anim->channels[i];
				file_write_uint8(file, chan->targetBone);
				file_write_uint32(file, chan->frameCount);
				for(int i = 0; i < chan->frameCount; i++)
				{
					FRAME const & frame = chan->frames[i];
					file_write_float(file, frame.time);
					file_write_vector(file, frame.position);
					file_write_quat(file, frame.rotation);
					file_write_vector(file, frame.scale);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////

	MESH * mesh_read(ACKFILE * file)
	{
		return Extension::load<MESH>(file);
	}

	void mesh_write(ACKFILE * file, MESH const * mesh)
	{
		Extension::writeHeader(file, TYPE_MESH, acff_guidMesh);

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
		file_write_uint32(file, mesh->lodMask);
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
		Extension::writeHeader(file, TYPE_MATERIAL, acff_guidMaterial);

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

		Extension::writeHeader(file, TYPE_BITMAP, acff_guidBitmap);

		int width, height, depth;
		int rs, gs, bs, as;
		int rt, gt, bt, at;

		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_DEPTH, &depth);
		engine_log("size: %d %d %d", width, height, depth);

		// Size in bits
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_RED_SIZE, &rs);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_GREEN_SIZE, &gs);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_BLUE_SIZE, &bs);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_ALPHA_SIZE, &as);
		engine_log("bpc: %d %d %d %d", rs, gs, bs, as);

		// GL_NONE, GL_SIGNED_NORMALIZED, GL_UNSIGNED_NORMALIZED, GL_FLOAT, GL_INT, and GL_UNSIGNED_INT
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_RED_TYPE, &rt);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_GREEN_TYPE, &gt);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_BLUE_TYPE, &bt);
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_ALPHA_TYPE, &at);
		// engine_log("types: %d %d %d %d", rt, gt, bt, at);
		assert(gs == 0 || rt == gt);
		assert(bs == 0 || rt == bt);
		assert(as == 0 || rt == at);

		int internalFormat;
		glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

		GLenum format = GL_RED;
		if(gt != GL_NONE) {
			format = GL_RG;
			if(bt != GL_NONE) {
				format = GL_RGB;
				if(at != GL_NONE) {
					format = GL_RGBA;
				}
			} else {
				assert(at == GL_NONE);
			}
		} else {
			assert(bt == GL_NONE);
			assert(at == GL_NONE);
		}

		GLenum type;
		switch(rs) {
			case 8:
				type = GL_UNSIGNED_BYTE;
				break;
			case 16:
				type = (rt == GL_FLOAT) ? GL_HALF_FLOAT : GL_UNSIGNED_SHORT;
				break;
			case 32:
				type = (rt == GL_FLOAT) ? GL_FLOAT : GL_UNSIGNED_INT;
				break;
			default:
				abort();
		}

		GLsizei bpp = (rs + gs + bs + as + 7 /*round up*/) / 8;
		engine_log(
			"bpp,iformat,format,type: %d %s %s %s",
			bpp,
			GLenumToString(internalFormat),
			GLenumToString(format),
			GLenumToString(type));
		GLsizei bufsiz = bpp * width * height * depth;

		void * pixels = malloc(bufsiz);

		assert(pixels != nullptr);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGetTextureImage(
			bitmap->object,
			0,
			format, type,
			bufsiz, pixels);

		file_write_uint32(file, bitmap->target);
		file_write_uint32(file, internalFormat);

		file_write_uint32(file, width);
		file_write_uint32(file, height);
		file_write_uint32(file, depth);

		file_write_uint32(file, format);
		file_write_uint32(file, type);
		file_write_uint32(file, bufsiz);
		file_write(file, pixels, bufsiz);

		free(pixels);
	}
}

MODEL * loadModel(ACKFILE * file, ACKGUID const * guid)
{
	assert(guid_compare(guid, &acff_guidModel));

	uint32_t boneCount = file_read_uint32(file);
	uint32_t meshCount = file_read_uint32(file);
	uint32_t animationCount = file_read_uint32(file);

	MODEL * result = model_create(meshCount, boneCount, animationCount);

	result->minimumLOD = file_read_uint32(file);

	for(uint i = 0; i < boneCount; i++)
	{
		BONE & dst = result->bones[i];
		file_read(file, dst.name, 64);
		dst.parent = file_read_uint8(file);
		dst.transform = file_read_matrix(file);
		dst.bindToBoneTransform = file_read_matrix(file);
	}

	for(uint i = 0; i < meshCount; i++)
	{
		result->meshes[i] = Extension::load<MESH>(file);
	}
	for(uint i = 0; i < meshCount; i++)
	{
		result->materials[i] = Extension::load<MATERIAL>(file);
	}

	for(uint i = 0; i < animationCount; i++)
	{
		char * name = file_read_string(file, 0);
		var duration = file_read_float(file);
		uint32_t flags = file_read_uint32(file);
		uint32_t chancnt = file_read_uint32(file);

		ANIMATION * anim = anim_create(name, chancnt);
		anim->duration = duration;
		anim->flags = flags;

		for(uint i = 0; i < anim->channelCount; i++)
		{
			uint8_t bone = file_read_uint8(file);
			uint32_t frameCount = file_read_uint32(file);

			CHANNEL * chan = chan_create(frameCount);
			chan->targetBone = bone;

			for(int i = 0; i < chan->frameCount; i++)
			{
				FRAME & frame = chan->frames[i];
				frame.time = file_read_float(file);
				frame.position = file_read_vector(file);
				frame.rotation = file_read_quat(file);
				frame.scale = file_read_vector(file);
			}

			anim->channels[i] = chan;
		}
		result->animations[i] = anim;
		free(name);
	}

	model_updateBoundingBox(result);

	return result;
}

static MESH * loadMesh(ACKFILE * file, ACKGUID const * guid)
{
	assert(guid_compare(guid, &acff_guidMesh));

	GLenum primitiveType = file_read_uint32(file);
	uint32_t indexCount  = file_read_uint32(file);
	uint32_t vertexCount = file_read_uint32(file);
	uint32_t lodmask     = file_read_uint32(file);

	BUFFER * vertexBuffer = nullptr;
	BUFFER * indexBuffer = nullptr;

	if(indexCount > 0)
	{
		indexBuffer = buffer_create(INDEXBUFFER);
		buffer_set(indexBuffer, indexCount * sizeof(INDEX), nullptr);
		INDEX * indices = (INDEX*)buffer_map(indexBuffer, WRITEONLY);
		for(uint i = 0; i < indexCount; i++)
		{
			indices[i] = file_read_uint32(file);
		}
		buffer_unmap(indexBuffer);
	}

	if(vertexCount > 0)
	{
		vertexBuffer = buffer_create(VERTEXBUFFER);
		buffer_set(vertexBuffer, vertexCount * sizeof(VERTEX), nullptr);
		VERTEX * vertices = (VERTEX*)buffer_map(vertexBuffer, WRITEONLY);
		for(uint i = 0; i < vertexCount; i++)
		{
			vertices[i].position = file_read_vector(file);
			vertices[i].normal = file_read_vector(file);
			vertices[i].tangent = file_read_vector(file);
			vertices[i].color = file_read_color(file);
			vertices[i].texcoord0 = file_read_uv(file);
			vertices[i].texcoord1 = file_read_uv(file);
			file_read(file, vertices[i].bones.values, 4);
			file_read(file, vertices[i].boneWeights.values, 4);
		}
		buffer_unmap(vertexBuffer);
	}

	MESH * result = mesh_create(primitiveType, vertexBuffer, indexBuffer);
	mesh_updateBoundingBox(result);
	result->lodMask = lodmask;
	return result;
}

static MATERIAL * loadMaterial(ACKFILE * file, ACKGUID const * guid)
{
	assert(guid_compare(guid, &acff_guidMaterial));

	MATERIAL * result = mtl_create();

	result->albedo = file_read_color(file);
	result->emission = file_read_color(file);
	result->roughness = file_read_float(file);
	result->metallic = file_read_float(file);
	result->fresnell = file_read_float(file);

	int mask = file_read_uint8(file);

	if(mask & 1) result->albedoTexture = Extension::load<BITMAP>(file);
	if(mask & 2) result->normalTexture = Extension::load<BITMAP>(file);
	if(mask & 4) result->attributeTexture = Extension::load<BITMAP>(file);
	if(mask & 8) result->emissionTexture = Extension::load<BITMAP>(file);

	return result;
}

static int getNumMimaps(int width, int height, int depth)
{
	int mips = 0;
	while(width > 0 && height > 0 && depth > 0) {
		width /= 2;
		height /= 2;
		depth /= 2;
		mips += 1;
	}
	return mips;
}

static BITMAP * loadBitmap(ACKFILE * file, ACKGUID const * guid)
{
	assert(guid_compare(guid, &acff_guidBitmap));

	GLenum target = file_read_uint32(file);
	GLenum format = file_read_uint32(file);

	uint width  = file_read_uint32(file);
	uint height = file_read_uint32(file);
	uint depth  = file_read_uint32(file);

	GLenum pixelFormat = file_read_uint32(file);
	GLenum pixelType = file_read_uint32(file);

	GLsizei bufsiz = file_read_uint32(file);

	void * pixels = malloc(bufsiz);
	assert(pixels != nullptr);
	assert(file_read(file, pixels, bufsiz) == bufsiz);

	BITMAP * result = bmap_create(target, format);

	result->width = width;
	result->height = height;
	result->depth = depth;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	switch(target)
	{
		case GL_TEXTURE_1D:
			glTextureStorage1D(
				result->object,
				1,
				format,
				width);
			glTextureSubImage1D(
				result->object,
				0, 0, width,
				pixelFormat, pixelType,
				pixels);
				break;
		case GL_TEXTURE_1D_ARRAY:
		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
			glTextureStorage2D(
				result->object,
				1,
				format,
				width, height);
			glTextureSubImage2D(
				result->object,
				0, 0, 0, width, height,
				pixelFormat, pixelType,
				pixels);
				break;
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_3D:
			glTextureStorage3D(
				result->object,
				1,
				format,
				width, height, depth);
			glTextureSubImage3D(
				result->object,
				0, 0, 0, 0, width, height, depth,
				pixelFormat, pixelType,
				pixels);
				break;
		default:
			abort();
	}

	engine_log("loaded: %s %s", GLenumToString(pixelFormat), GLenumToString(pixelType));

	if(pixelFormat == GL_RGBA && pixelType == GL_UNSIGNED_BYTE) {
		result->pixels = pixels;
	} else {
		free(pixels);
	}

	return result;
}

EXTENSION acknextDefaultSerializers =
{
	canLoad : [](ACKGUID const * guid)
    {
		if(guid_compare(guid, &acff_guidBitmap)) return TYPE_BITMAP;
        if(guid_compare(guid, &acff_guidMaterial)) return TYPE_MATERIAL;
        if(guid_compare(guid, &acff_guidMesh)) return TYPE_MESH;
        if(guid_compare(guid, &acff_guidModel)) return TYPE_MODEL;
        if(guid_compare(guid, &acff_guidShader)) return TYPE_SHADER;
        return TYPE_INVALID;
    },
    loadModel : &::loadModel,
    loadShader : nullptr,
    loadMaterial : loadMaterial,
    loadSound : nullptr,
    loadHull : nullptr,
    loadBlob : nullptr,
    loadBitmap : loadBitmap,
    loadBuffer : nullptr,
    loadMesh : &::loadMesh,
    loadView : nullptr,
    loadLight : nullptr,
};
