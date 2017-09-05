#include <acknext.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl3w.h>

#include <assert.h>

#include "../l3dt/l3dt.h"

void debug_tools();

MESH * mesh;
SHADER * shdTerrain;

bool showWframe = false;

int iSubdivision = 1;

extern var debug_movement;
extern var debug_speedup;

void twframe()
{
	showWframe = !showWframe;
}

void more()
{
	int max;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &max);
	if(iSubdivision < max)
		iSubdivision ++;
	shader_setvar(shdTerrain, "iSubdivision", GL_INT, (int[]){iSubdivision});
}

void less()
{
	if(iSubdivision > 0)
		iSubdivision --;
	shader_setvar(shdTerrain, "iSubdivision", GL_INT, (int[]){iSubdivision});
}

void storepos()
{
	if(key_lctrl)
	{
		ACKFILE * file = file_open_write("camera.dat");
		file_write(file, camera, sizeof(CAMERA));
		file_close(file);
	}
}

void gamemain()
{
	debug_speedup = 500;

	view_create((RENDERCALL)render_scene_with_camera, camera);
	task_defer((ENTRYPOINT)debug_tools, NULL);
	event_attach(on_escape, (EVENTHANDLER)engine_shutdown);
	filesys_addResource("/home/felix/projects/acknext/project-z/resources/", "/");

	event_attach(on_comma, twframe);
	event_attach(on_kp_plus, more);
	event_attach(on_kp_minus, less);
	event_attach(on_s, storepos);

	{
		ACKFILE * file = file_open_read("camera.dat");
		if(file) {
			file_read(file, camera, sizeof(CAMERA));
			file_close(file);
		}
	}

	BLOB * packed = blob_load("/terrain/GrassyMountains_HF.hfz");
	engine_log("Packed size: %lu", packed->size);

	BLOB * unpacked = blob_inflate(packed);
	engine_log("Unpacked size: %lu", unpacked->size);

	blob_remove(packed);

	L3Heightfield * hf = l3hf_decode(unpacked->data, unpacked->size);

	BITMAP * heightmapTexture = bmap_create(GL_TEXTURE_2D, GL_R32F);

	bmap_set(heightmapTexture, hf->width, hf->height, GL_RED, GL_FLOAT, hf->data);

	bmap_to_mipmap(heightmapTexture);

	GLuint hmp = heightmapTexture->object;
	glTextureParameteri(hmp, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(hmp, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(hmp, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	const int subdiv = 6;
	const int tilesX = (1<<subdiv);
	const int tilesY = (1<<subdiv);
	const int sizeX = hf->width / tilesX;
	const int sizeY = hf->height / tilesY;

	engine_log("(%d,%d) → (%d, %d)", tilesX, tilesY, sizeX, sizeY);

	BUFFER * indexBuffer = buffer_create(INDEXBUFFER);
	buffer_set(indexBuffer, 4 * sizeof(INDEX) * tilesX * tilesY, NULL);
	INDEX * indices = buffer_map(indexBuffer, WRITEONLY);

	const int stride = hf->width;
	const int delta = 1;
	for(int y = 0; y < tilesY; y++)
	{
		for(int x = 0; x < tilesX; x++)
		{
			INDEX * face = &indices[4 * (y * tilesX + x)];
			int base = stride * y + delta * x;
			face[0] = base + 0;
			face[1] = base + delta;
			face[2] = base + stride + delta;
			face[3] = base + stride;
		}
	}

	buffer_unmap(indexBuffer);

	mesh = mesh_create(GL_QUADS, NULL, indexBuffer);
	{
		mesh_setvar(mesh, "vecTerrainSize", GL_INT_VEC2, (int[]){ hf->width, hf->height });
		mesh_setvar(mesh, "vecTileSize", GL_INT_VEC2, (int[]){ sizeX, sizeY });
		mesh_setvar(mesh, "texHeightmap", GL_SAMPLER_2D, &heightmapTexture);
		mesh_setvar(mesh, "fTerrainScale", GL_FLOAT, &hf->horizontalScale);

		BITMAP * bmpNM    = bmap_to_mipmap(bmap_load("/terrain/GrassyMountains_TN.png"));
		mesh_setvar(mesh, "texNormalMap", GL_SAMPLER_2D, &bmpNM);
	}

	shdTerrain = shader_create();
	{
		shader_addFileSource(shdTerrain, VERTEXSHADER, "shaders/terrain.vert");
		shader_addFileSource(shdTerrain, TESSCTRLSHADER, "shaders/terrain.tesc");
		shader_addFileSource(shdTerrain, TESSEVALSHADER, "shaders/terrain.tese");
		shader_addFileSource(shdTerrain, FRAGMENTSHADER, "shaders/terrain.frag");
		shader_addFileSource(shdTerrain, FRAGMENTSHADER, "/builtin/shaders/lighting.glsl");
		shader_addFileSource(shdTerrain, FRAGMENTSHADER, "/builtin/shaders/gamma.glsl");
		shader_addFileSource(shdTerrain, FRAGMENTSHADER, "/builtin/shaders/ackpbr.glsl");
		shader_link(shdTerrain);

		glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &iSubdivision);
		engine_log("Maximum subdivision: %d", iSubdivision);
		shader_setvar(shdTerrain, "iSubdivision", GL_INT, (int[]){iSubdivision});
	}

	MODEL * model = model_create(1, 0, 0);
	{
		model->meshes[0] = mesh;
		model->materials[0] = mtl_create();

		model->materials[0]->shader = shdTerrain;

		BITMAP * bmpLSC   = bmap_to_mipmap(bmap_load("/terrain/GrassyMountains_TX.jpg"));
		BITMAP * bmpDetail = bmap_to_mipmap(bmap_load("/textures/grass-01.jpg"));

#define	GL_TEXTURE_MAX_ANISOTROPY_EXT          0x84FE
#define	GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT      0x84FF

		{
			GLfloat fLargest;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);

			engine_log("max anisotropy: %f", fLargest);

			glTextureParameterf(
				bmpLSC->object,
				GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
			glTextureParameterf(
				bmpDetail->object,
				GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
		}

		shader_setvar(model->materials[0], "texLargeScaleColor", GL_SAMPLER_2D, &bmpLSC);
		shader_setvar(model->materials[0], "texDetail", GL_SAMPLER_2D, &bmpDetail);
	}

	ENTITY * ent = ent_create(NULL, vector(0,0,0), NULL);
	ent->model = model;

	packed = blob_load("/terrain/GrassyMountains_AM.amf.gz");
	BLOB * attributefield = blob_inflate(packed);
	blob_remove(packed);

	L3Attributefield * am = l3af_decode(attributefield->data, attributefield->size);
	if(am->width != hf->width || am->height != hf->height) {
		abort();
	}

	BITMAP * amtexture = bmap_create(GL_TEXTURE_2D, GL_RG8UI);
	bmap_set(amtexture,
		am->width, am->height,
		GL_RG_INTEGER, GL_UNSIGNED_BYTE,
		am->data);
	shader_setvar(model->materials[0], "texTerrainMaterial", GL_UNSIGNED_INT_SAMPLER_2D, &amtexture);

	BITMAP * materials = bmap_create(GL_TEXTURE_2D_ARRAY, GL_RGBA8);
	{
		glTextureStorage3D(
			materials->object,
			10,
			materials->format,
			1024,
			1024,
			18);
		struct { int index; char const * file; } maps[] =
		{
			{ 0x0B, "/textures/TexturesCom_Grass0160_1_seamless_S.jpg" },
			{ 0x0C, "/textures/TexturesCom_Grass0169_1_seamless_S.jpg" },
			{ 0x0E, "/textures/TexturesCom_Grass0157_1_seamless_S.jpg" },
			{ 0x0F, "/textures/TexturesCom_GrassDead0101_1_seamless_S.jpg" },
			{ 0x10, "/textures/TexturesCom_Cliffs0167_1_seamless_S.jpg" },
			{ 0x11, "/textures/TexturesCom_Cliffs0177_1_seamless_S.jpg" },
			{ 0, NULL },
		};
		for(int i = 0; maps[i].file; i++)
		{
			BITMAP * source = bmap_load(maps[i].file);
			assert(source && source->width == 1024 && source->height == 1024);
			glTextureSubImage3D(
				materials->object,
				0,
				0, 0, maps[i].index,
				1024, 1024, 1,
				GL_RGBA, GL_UNSIGNED_BYTE,
				source->pixels);
			bmap_remove(source);
		}
		glGenerateTextureMipmap(materials->object);
		shader_setvar(model->materials[0], "texTerrainMaterials", GL_SAMPLER_2D_ARRAY, &materials);
	}

	shader_logInfo(shdTerrain);

	obj_listvar(shdTerrain);

	// Äh wut, aber ja, ist sinnvoller so im moment
	shader_setUniforms(shdTerrain, shdTerrain);

	while(true)
	{
		if(key_4) {
			engine_log("%f ms / %f FPS", 1000.0 * time_step, 1.0 / time_step);
		}
		task_yield();
	}

}

int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}
