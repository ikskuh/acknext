#include "model.h"
#include "engine.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model()
{

}

Model::~Model()
{

}

static std::unordered_map<std::string, std::unique_ptr<Model>> models;

static VECTOR ass_to_ack(const aiVector3D & vec)
{
	return (VECTOR){ vec.x, vec.y, vec.z };
}

Model * model_load(std::string const & file)
{
	using namespace Assimp;

	Importer importer;

	unsigned int flags =
		  aiProcess_JoinIdenticalVertices
		| aiProcess_CalcTangentSpace
		| aiProcess_Triangulate
	    | aiProcess_GenSmoothNormals
	    | aiProcess_PreTransformVertices // REMOVE THIS LATER!
	    | aiProcess_LimitBoneWeights
		| aiProcess_ValidateDataStructure
		| aiProcess_ImproveCacheLocality
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_OptimizeMeshes
		| aiProcess_OptimizeGraph
		// | aiProcess_FlipUVs
		;

	aiScene const * scene = importer.ReadFile(file,	flags);

	if(scene == nullptr) {
		engine_seterror(INVALID_OPERATION, importer.GetErrorString());
		return nullptr;
	}

	// The output scene still contains nodes, however there is only a root node
	// with children, each one referencing only one mesh, and each mesh
	// referencing one material. For rendering, you can simply render all meshes
	// in order - you don't need to pay attention to local transformations and
	// the node hierarchy. Animations are removed during this step. This step
	// is intended for applications without a scenegraph. The step CAN cause
	// some problems: if e.g. a mesh of the asset contains normals and another,
	// using the same material index, does not, they will be brought together,
	// but the first meshes's part of the normal list is zeroed. However, these
	// artifacts are rare.

	Model * model = new Model();

	if(scene->HasMaterials())
	{
		model->materials.resize(scene->mNumMaterials);
		for(unsigned int idx = 0; idx < scene->mNumMaterials; idx++)
		{
			MATERIAL & dst = model->materials[idx];
			aiMaterial const & src = *scene->mMaterials[idx];

			memset(&dst, 0, sizeof(MATERIAL));
			dst.color = (COLOR){1,1,1,1};
			dst.roughness = 1.0;

			aiColor4D cDiffuse, cEmissive;

			if(src.Get(AI_MATKEY_COLOR_DIFFUSE, cDiffuse) == aiReturn_SUCCESS) {
				dst.color.red = cDiffuse.r;
				dst.color.green = cDiffuse.g;
				dst.color.blue = cDiffuse.b;
				dst.color.alpha = cDiffuse.a;
			}

			if(src.Get(AI_MATKEY_COLOR_DIFFUSE, cEmissive) == aiReturn_SUCCESS) {
				dst.emission.red = cEmissive.r;
				dst.emission.green = cEmissive.g;
				dst.emission.blue = cEmissive.b;
				dst.emission.alpha = cEmissive.a;
			}

			float fShininess, fReflectivity;
			if(src.Get(AI_MATKEY_REFLECTIVITY, fReflectivity) == aiReturn_SUCCESS) {
				dst.metallic = fReflectivity;
			}
			if(src.Get(AI_MATKEY_SHININESS, fShininess) == aiReturn_SUCCESS) {
				dst.roughness = fShininess;
			}

			aiString path;
			if(src.GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS) {
				engine_log("Texture: %s", path.C_Str());

				dst.colorTexture = bmap_load("skin.bmp");
			}
		}
	}

	model->meshes.resize(scene->mNumMeshes);
	for(unsigned int idx = 0; idx < scene->mNumMeshes; idx++)
	{
		MESH * dst = &model->meshes[idx];
		aiMesh * src = scene->mMeshes[idx];

		if(src->mPrimitiveTypes & ~aiPrimitiveType_TRIANGLE) {
			engine_log("Invalid primitive type detected!");
			abort();
		}

		if(!src->HasNormals()) {
			engine_log("Mesh is missing normals!");
			abort();
		}

		if(!src->HasTangentsAndBitangents()) {
			engine_log("Mesh is missing normals!");
			abort();
		}

		dst->indexBuffer = buffer_create(INDEX_BUFFER);
		dst->vertexBuffer = buffer_create(VERTEX_BUFFER);
		dst->material = &model->materials[src->mMaterialIndex];

		{
			VERTEX * vertices = new VERTEX[src->mNumVertices];

			for(unsigned int i = 0; i < src->mNumVertices; i++) {
				auto const & pos = src->mVertices[i];
				auto & dst = vertices[i];

				dst.position = ass_to_ack(pos);

				auto const & normal = src->mNormals[i];
				dst.normal = ass_to_ack(normal);

				auto const & tanget = src->mTangents[i];
				dst.tangent = ass_to_ack(tanget);

				// Initialize vertex with white color as
				// default color
				dst.color = (COLOR){1,1,1,1};
			}

			if(src->HasTextureCoords(0))
			{
				for(unsigned int i = 0; i < src->mNumVertices; i++) {
					auto const & vec = src->mTextureCoords[0][i];
					auto & dst = vertices[i];
					dst.texcoord0.u = vec.x;
					dst.texcoord0.v = vec.y;
				}
			}
			if(src->HasTextureCoords(1))
			{
				for(unsigned int i = 0; i < src->mNumVertices; i++) {
					auto const & vec = src->mTextureCoords[1][i];
					auto & dst = vertices[i];
					dst.texcoord1.u = vec.x;
					dst.texcoord1.v = vec.y;
				}
			}
			if(src->HasVertexColors(0))
			{
				for(unsigned int i = 0; i < src->mNumVertices; i++) {
					auto const & vec = src->mColors[0][i];
					auto & dst = vertices[i];
					dst.color.red = vec.r;
					dst.color.green = vec.g;
					dst.color.blue = vec.b;
					dst.color.alpha = vec.a;
				}
			}

			buffer_set(
				dst->vertexBuffer,
				3 * src->mNumVertices * sizeof(VERTEX),
				vertices);
			delete vertices;
		}
		{
			INDEX * indices = new INDEX[3 * src->mNumFaces];
			for(unsigned int i = 0; i < src->mNumFaces; i++) {
				aiFace const & face = src->mFaces[i];
				for(int j = 0; j < 3; j++) {
					indices[3 * i + j] = face.mIndices[j];
				}
			}
			buffer_set(
				dst->indexBuffer,
				3 * src->mNumFaces * sizeof(INDEX),
				indices);
			delete indices;
		}
	}

	if(scene->mRootNode->mNumChildren > 0) {
		engine_log("Models with graph structure are not supported yet!");
		// TODO: Add node graph to model structure
		abort();
	}

	return model;
}

Model * model_get(char const * name)
{
	std::string str(name);
	auto it = models.find(str);
	if(it != models.end()) {
		return it->second.get();
	}
	Model * model = model_load(str);
	models.emplace(name, model);
	return model;
}
