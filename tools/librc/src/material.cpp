#include "librc.h"
#include <json.hpp>
#include <fstream>
#include <stdlib.h>
#include <string.h>

char const * strdup(const std::string & str)
{
	return strdup(str.c_str());
}

C_API void librc_write_mtl(ACKFILE * outfile, rcMaterial const * mtl)
{
	assert(outfile);
	assert(mtl);

	file_write_header(outfile, TYPE_MATERIAL, acff_guidMaterial);

	file_write_color(outfile, mtl->albedo);
	file_write_color(outfile, mtl->emission);
	file_write_float(outfile, mtl->roughness);
	file_write_float(outfile, mtl->metalness);
	file_write_float(outfile, mtl->fresnell);

	uint8_t mask = 0;
	if(mtl->albedoTexture) mask |= 1;
	if(mtl->normalTexture) mask |= 2;
	if(mtl->attributeTexture) mask |= 4;
	if(mtl->emissionTexture) mask |= 8;

	file_write_uint8(outfile, mask);

	if(mtl->embedTextures)
	{
#define LOADWRITE(_mask,_attrib) \
			if(mask & _mask) \
			{ \
				rcImage * tex = librc_load_image(mtl->_attrib); \
				assert(tex); \
				librc_write_image(outfile, tex); \
				librc_free_image(tex); \
			}
		LOADWRITE(1, albedoTexture)
		LOADWRITE(2, normalTexture)
		LOADWRITE(4, attributeTexture)
		LOADWRITE(8, emissionTexture)
#undef LOADWRITE
	}
	else
	{
		if(mask & 1) file_write_symlink(outfile, mtl->albedoTexture,    mtl->shareTextures);
		if(mask & 2) file_write_symlink(outfile, mtl->normalTexture,    mtl->shareTextures);
		if(mask & 4) file_write_symlink(outfile, mtl->attributeTexture, mtl->shareTextures);
		if(mask & 8) file_write_symlink(outfile, mtl->emissionTexture,  mtl->shareTextures);
	}
}

// read from "real" filesys
C_API rcMaterial * librc_load_mtl(char const * fileName)
{
	assert(fileName);

	using namespace nlohmann;

	json mtldef;
	{
		std::ifstream i(fileName);
		i >> mtldef;
	}
	if(!mtldef.is_object()) {
		LOG("%s is not a valid json file.", fileName);
		return nullptr;
	}

	if(!mtldef["type"].is_string() || mtldef["type"] != "acknext-material") {
		LOG("%s does not contain an acknext material definition.", fileName);
		return nullptr;
	}

	rcMaterial * mtl = (rcMaterial*)malloc(sizeof(rcMaterial));

	mtl->embedTextures = false;
	mtl->shareTextures = true;

	mtl->albedo = { 1, 1, 1, 1 };
	mtl->emission = { 0, 0, 0, 1 };
	mtl->roughness = 0.8;
	mtl->metalness = 0.0;
	mtl->fresnell = 0.0;

	mtl->albedoTexture = nullptr;
	mtl->normalTexture = nullptr;
	mtl->attributeTexture = nullptr;
	mtl->emissionTexture = nullptr;

	/**************************************************/

	if(mtldef["albedo"].is_array())
	{
		json & arr = mtldef["albedo"];
		assert(arr.size() == 3);
		mtl->albedo.red = arr[0];
		mtl->albedo.green = arr[1];
		mtl->albedo.blue = arr[2];
	}
	else assert	(mtldef["albedo"].is_null());

	/**************************************************/

	if(mtldef["emission"].is_array())
	{
		json & arr = mtldef["emission"];
		assert(arr.size() == 3);
		mtl->emission.red = arr[0];
		mtl->emission.green = arr[1];
		mtl->emission.blue = arr[2];
	}
	else assert	(mtldef["emission"].is_null());

	/**************************************************/

	if(mtldef["roughness"].is_number())
	{
		mtl->roughness = mtldef["roughness"];
	}
	else assert	(mtldef["roughness"].is_null());

	/**************************************************/

	if(mtldef["metalness"].is_number())
	{
		mtl->metalness = mtldef["metalness"];
	}
	else assert	(mtldef["metalness"].is_null());

	/**************************************************/

	if(mtldef["fresnell"].is_number())
	{
		mtl->fresnell = mtldef["fresnell"];
	}
	else assert	(mtldef["fresnell"].is_null());

	/**************************************************/

	if(mtldef["textures"].is_object())
	{
		json & textures = mtldef["textures"];

		if(textures["embed"].is_boolean())
			mtl->embedTextures = textures["embed"];
		else
			assert(textures["embed"].is_null());

		if(textures["share"].is_boolean())
			mtl->shareTextures = textures["share"];
		else
			assert(textures["share"].is_null());

		if(textures["albedo"].is_string())
			mtl->albedoTexture = strdup(textures["albedo"]);
		else
			assert(textures["albedo"].is_null());

		if(textures["normal"].is_string())
			mtl->normalTexture = strdup(textures["normal"]);
		else
			assert(textures["normal"].is_null());

		if(textures["attribute"].is_string())
			mtl->attributeTexture = strdup(textures["attribute"]);
		else
			assert(textures["attribute"].is_null());

		if(textures["emission"].is_string())
			mtl->emissionTexture = strdup(textures["emission"]);
		else
			assert(textures["emission"].is_null());

	}
	else assert	(mtldef["textures"].is_null());

	/**************************************************/

	if(mtl->albedo.red < 0.0 || mtl->albedo.green < 0.0 || mtl->albedo.blue < 0.0) {
		LOG("warning: albedo has negative color values. May be undesired!\n");
	}

	if(mtl->emission.red < 0.0 || mtl->emission.green < 0.0 || mtl->emission.blue < 0.0) {
		LOG("warning: emission has negative color values. May be undesired!\n");
	}

	if(mtl->roughness < 0.0 || mtl->roughness > 1.0) {
		LOG("warning: roughness must be between 0.0 and 1.0!\n");
	}

	if(mtl->metalness < 0.0 || mtl->metalness > 1.0) {
		LOG("warning: metalness must be between 0.0 and 1.0!\n");
	}

	return mtl;
}

// deletes all strings, then the mtl
C_API void librc_free_mtl(rcMaterial * mtl)
{
	assert(mtl);
	if(mtl->albedoTexture)
		free((char*)mtl->albedoTexture);
	if(mtl->normalTexture)
		free((char*)mtl->normalTexture);
	if(mtl->attributeTexture)
		free((char*)mtl->attributeTexture);
	if(mtl->emissionTexture)
		free((char*)mtl->emissionTexture);
	free(mtl);
}
