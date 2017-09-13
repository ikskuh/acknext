#include <stdio.h>
#include <stdlib.h>

#include <acknext.h>
#include <acknext/serialization.h>
#include <acknext/acff.h>

#include "json.hpp"
#include <fstream>

int process_material(char const * infile, ACKFILE * outfile)
{
	using namespace nlohmann;

	json mtldef;
	{
		std::ifstream i(infile);
		i >> mtldef;
	}
	if(!mtldef.is_object()) {
		fprintf(stderr, "%s is not a valid json file.", infile);
		return EXIT_FAILURE;
	}

	if(!mtldef["type"].is_string() || mtldef["type"] != "acknext-material") {
		fprintf(stderr, "%s does not contain an acknext material definition.", infile);
		return EXIT_FAILURE;
	}

	bool embedTextures = false;
	bool shareTextures = true;

	COLOR albedo = { 1, 1, 1, 1 };
	COLOR emission = { 0, 0, 0, 1 };
	float roughness = 0.8;
	float metalness = 0.0;
	float fresnell = 0.0;

	std::string albedoTexture;
	std::string normalTexture;
	std::string attributeTexture;
	std::string emissionTexture;

	/**************************************************/

	if(mtldef["albedo"].is_array())
	{
		json & arr = mtldef["albedo"];
		assert(arr.size() == 3);
		albedo.red = arr[0];
		albedo.green = arr[1];
		albedo.blue = arr[2];
	}
	else assert	(mtldef["albedo"].is_null());

	/**************************************************/

	if(mtldef["emission"].is_array())
	{
		json & arr = mtldef["emission"];
		assert(arr.size() == 3);
		emission.red = arr[0];
		emission.green = arr[1];
		emission.blue = arr[2];
	}
	else assert	(mtldef["emission"].is_null());

	/**************************************************/

	if(mtldef["roughness"].is_number())
	{
		roughness = mtldef["roughness"];
	}
	else assert	(mtldef["roughness"].is_null());

	/**************************************************/

	if(mtldef["metalness"].is_number())
	{
		metalness = mtldef["metalness"];
	}
	else assert	(mtldef["metalness"].is_null());

	/**************************************************/

	if(mtldef["fresnell"].is_number())
	{
		fresnell = mtldef["fresnell"];
	}
	else assert	(mtldef["fresnell"].is_null());

	/**************************************************/

	if(mtldef["textures"].is_object())
	{
		json & textures = mtldef["textures"];

		if(textures["embed"].is_boolean())
			embedTextures = textures["embed"];
		else
			assert(textures["embed"].is_null());

		if(textures["share"].is_boolean())
			shareTextures = textures["share"];
		else
			assert(textures["share"].is_null());

		if(textures["albedo"].is_string())
			albedoTexture = textures["albedo"];
		else
			assert(textures["albedo"].is_null());

		if(textures["normal"].is_string())
			normalTexture = textures["normal"];
		else
			assert(textures["normal"].is_null());

		if(textures["attributes"].is_string())
			attributeTexture = textures["attributes"];
		else
			assert(textures["attributes"].is_null());

		if(textures["emission"].is_string())
			emissionTexture = textures["emission"];
		else
			assert(textures["emission"].is_null());

	}
	else assert	(mtldef["textures"].is_null());

	/**************************************************/

	if(albedo.red < 0.0 || albedo.green < 0.0 || albedo.blue < 0.0) {
		fprintf(stderr, "warning: albedo has negative color values. May be undesired!\n");
	}

	if(emission.red < 0.0 || emission.green < 0.0 || emission.blue < 0.0) {
		fprintf(stderr, "warning: emission has negative color values. May be undesired!\n");
	}

	if(roughness < 0.0 || roughness > 1.0) {
		fprintf(stderr, "warning: roughness must be between 0.0 and 1.0!\n");
	}

	if(metalness < 0.0 || metalness > 1.0) {
		fprintf(stderr, "warning: metalness must be between 0.0 and 1.0!\n");
	}

	if(embedTextures) {
		fprintf(stderr, "warning: Embedding textures is not supported yet, will use referenced textures!\n");
	}

	/**************************************************/

	// printf("albedo:   %f %f %f\n", albedo.red, albedo.green, albedo.blue);
	// printf("emission: %f %f %f\n", emission.red, emission.green, emission.blue);
	// printf("attribs:  %f %f %f\n", roughness, metalness, fresnell);
	// printf("textures: `%s` `%s` `%s` `%s`\n", albedoTexture.c_str(), normalTexture.c_str(), attributeTexture.c_str(), emissionTexture.c_str());

	file_write_header(outfile, TYPE_MATERIAL, acff_guidMaterial);

	file_write_color(outfile, albedo);
	file_write_color(outfile, emission);
	file_write_float(outfile, roughness);
	file_write_float(outfile, metalness);
	file_write_float(outfile, fresnell);

	uint8_t mask = 0;
	if(!albedoTexture.empty()) mask |= 1;
	if(!normalTexture.empty()) mask |= 2;
	if(!attributeTexture.empty()) mask |= 4;
	if(!emissionTexture.empty()) mask |= 8;

	file_write_uint8(outfile, mask);

	if(mask & 1) file_write_symlink(outfile, albedoTexture.c_str(), shareTextures);
	if(mask & 2) file_write_symlink(outfile, normalTexture.c_str(), shareTextures);
	if(mask & 4) file_write_symlink(outfile, attributeTexture.c_str(), shareTextures);
	if(mask & 8) file_write_symlink(outfile, emissionTexture.c_str(), shareTextures);

	return EXIT_SUCCESS;
}
