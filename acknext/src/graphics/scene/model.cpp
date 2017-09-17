#include "model.hpp"

#include <string>
#include <unordered_map>
#include <assert.h>
#include <float.h>

#include "../../extensions/extension.hpp"

static std::unordered_map<std::string, MODEL*> modelCache;

Model::Model() :
    EngineObject<MODEL>(),
    userCreated(true)
{

}

Model::~Model()
{
	/*
	for(MESH & mesh : meshes) {
		buffer_remove(mesh.indexBuffer);
		buffer_remove(mesh.vertexBuffer);
	}
	for(MATERIAL & mtl : materials) {
		bmap_remove(mtl.colorTexture);
		bmap_remove(mtl.emissionTexture);
		bmap_remove(mtl.attributeTexture);
	}
	*/
}

ACKNEXT_API_BLOCK
{
	MODEL * model_create(int numMeshes, int numBones, int numAnimations)
	{
		Model * model = new Model();
		MODEL & api = model->api();

		api.minimumLOD = 16;

		api.meshCount = maxv(numMeshes, 1);
		api.boneCount = maxv(numBones, 1);
		api.animationCount = maxv(numAnimations, 0);

		api.meshes = (MESH**)malloc(sizeof(MESH*)*api.meshCount);
		api.materials = (MATERIAL**)malloc(sizeof(MATERIAL*)*api.meshCount);

		memset(api.meshes, 0, sizeof(MESH*)*api.meshCount);
		memset(api.materials, 0, sizeof(MESH*)*api.meshCount);

		if(api.animationCount > 0)
		{
			api.animations = (ANIMATION**)malloc(sizeof(ANIMATION*)*api.animationCount);
			memset(api.animations, 0, sizeof(MESH*)*api.animationCount);
		}

		for(int i = 0; i < ACKNEXT_MAX_BONES; i++) {
			api.bones[i].parent = 0;
			strcpy(api.bones[i].name, "");
			mat_id(&api.bones[i].transform);
			mat_id(&api.bones[i].bindToBoneTransform);
		}

		return demote(model);
	}

	MODEL * model_get(char const * fileName) // uses caching
	{
		std::string name(fileName);
		auto it = modelCache.find(name);
		if(it != modelCache.end()) {
			return (*it).second;
		}
		MODEL * model = model_load(fileName);
		if(model != nullptr) {
			promote<Model>(model)->userCreated = false;
			modelCache.emplace(name, model);
		}
		return model;
	}

	void model_remove(MODEL * _model) // only created or loaded ones
	{
		Model * model = promote<Model>(_model);
		if(model && model->userCreated) {
			delete model;
		}
	}

	MODEL * model_load(const char *fileName)
	{
		return Extension::load<MODEL>(file_open_read(fileName));
	}

	void model_updateBoundingBox(MODEL * model)
	{
		ARG_NOTNULL(model, );

		AABB & aabb = model->boundingBox;
		aabb.maximum = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		aabb.minimum = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
		for(int i = 0; i < model->meshCount; i++)
		{
			auto * mesh = model->meshes[i];
			mesh_updateBoundingBox(mesh);

			aabb.maximum.x = maxv(aabb.maximum.x, mesh->boundingBox.maximum.x);
			aabb.maximum.y = maxv(aabb.maximum.y, mesh->boundingBox.maximum.y);
			aabb.maximum.z = maxv(aabb.maximum.z, mesh->boundingBox.maximum.z);

			aabb.minimum.x = minv(aabb.minimum.x, mesh->boundingBox.minimum.x);
			aabb.minimum.y = minv(aabb.minimum.y, mesh->boundingBox.minimum.y);
			aabb.minimum.z = minv(aabb.minimum.z, mesh->boundingBox.minimum.z);
		}
	}
}
