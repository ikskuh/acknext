#include <acknext.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stack>
#include <vector>
#include <map>
#include <unordered_map>
#include <assert.h>

char buf[256];

extern "C"  void putmat(char const * title, MATRIX const * mat);

void ai_to_ack(MATRIX * dst, aiMatrix4x4 const & src)
{
	var array[4][4] = {
		{ src.a1, src.b1, src.c1, src.d1 },
	    { src.a2, src.b2, src.c2, src.d2 },
	    { src.a3, src.b3, src.c3, src.d3 },
	    { src.a4, src.b4, src.c4, src.d4 },
	};
	memcpy(dst->fields, array, sizeof(var) * 16);
}

VECTOR ai_to_ack(aiVector3D const & src) {
	return (VECTOR) { src.x, src.y, src.z };
}

QUATERNION ai_to_ack(aiQuaternion const & src) {
	return (QUATERNION) { src.x, src.y, src.z, src.w };
}

void printNodes(aiNode const * node, char const * indent = "")
{
	char buffer[256];
	strcpy(buffer, "");
	if(node->mNumMeshes > 0)
	{
		strcpy(buffer, "(");
		for(uint i = 0; i < node->mNumMeshes; i++) {
			if(i > 0) strcat(buffer,", ");
			std::sprintf(buf, "%d", i);
			strcat(buffer, buf);
		}
		strcat(buffer, ")");
	}

	MATRIX nodeTransform;
	ai_to_ack(&nodeTransform, node->mTransformation);

	engine_log("%s%s%s", indent, node->mName.C_Str(), buffer);

	if(node->mNumMeshes > 0)
	{
		putmat("Trafo:", &nodeTransform	);
	}

	/*
	engine_log("trafo:\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f",
		nodeTransform.fields[0][0], nodeTransform.fields[0][1], nodeTransform.fields[0][2], nodeTransform.fields[0][3],
		nodeTransform.fields[1][0], nodeTransform.fields[1][1], nodeTransform.fields[1][2], nodeTransform.fields[1][3],
		nodeTransform.fields[2][0], nodeTransform.fields[2][1], nodeTransform.fields[2][2], nodeTransform.fields[2][3],
		nodeTransform.fields[3][0], nodeTransform.fields[3][1], nodeTransform.fields[3][2], nodeTransform.fields[3][3]);
	*/
	VECTOR4 vec = { 0, 0, 0, 1 };
	vec4_transform(&vec, &nodeTransform);

	// engine_log("%s%f %f %f %f", indent, vec.x, vec.y, vec.z, vec.w);

	strcpy(buffer, indent);
	strcat(buffer, "  ");
	if(node->mNumChildren > 0 && node->mChildren)
	{
		for(uint i = 0; i < node->mNumChildren; i++)
		{
			printNodes(node->mChildren[i], buffer);
		}
	}
}

void translateNodes(MODEL * target, aiNode const * node, uint8_t parent, uint8_t & index)
{
	uint8_t id = index++;
	BONE & bone = target->bones[id];
	bone.parent = parent;
	ai_to_ack(&bone.transform, node->mTransformation);
	strcpy(bone.name, node->mName.C_Str());
	mat_id(&bone.bindToBoneTransform);
	for(uint i = 0; i < node->mNumChildren; i++) {
		translateNodes(target, node->mChildren[i], id, index);
	}
}

using namespace Assimp;
Importer importer;

aiScene const  * scene;

extern "C" void animate(MODEL * model, char const * name, double frameTime)
{
	std::map<std::string, BONE&> bones;
	for(int i = 0; i < model->boneCount; i++) {
		bones.emplace(std::string(model->bones[i].name), model->bones[i]);
	}
	aiAnimation * anim = nullptr;
	for(uint i = 0; i < scene->mNumAnimations; i++) {
		if(scene->mAnimations[i]->mName == aiString(name)) {
			anim = scene->mAnimations[i];
			break;
		}
	}
	assert(anim);

	if(anim->mTicksPerSecond > 0) {
		frameTime *= anim->mTicksPerSecond;
	} else {
		frameTime *= 25.0;
	}

	if(anim->mDuration > 0) {
		frameTime = fmod(frameTime, anim->mDuration);
	}

	for(uint i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim * chan = anim->mChannels[i];
		try {
			BONE & bone = bones.at(std::string(chan->mNodeName.C_Str()));

			assert(aiString(bone.name) == chan->mNodeName);

			VECTOR pos = nullvector;
			QUATERNION rot = *quat_id(NULL);
			VECTOR scale = { 1, 1, 1 };

			if(chan->mNumPositionKeys > 0)
			{
				pos = ai_to_ack(chan->mPositionKeys[0].mValue);
				for(uint j = 1; j < chan->mNumPositionKeys; j++) {
					if(chan->mPositionKeys[j].mTime > frameTime) break;
					pos = ai_to_ack(chan->mPositionKeys[j].mValue);
				}
			}

			if(chan->mNumScalingKeys > 0)
			{
				scale = ai_to_ack(chan->mScalingKeys[0].mValue);
				for(uint j = 1; j < chan->mNumScalingKeys; j++) {
					if(chan->mScalingKeys[j].mTime > frameTime) break;
					scale = ai_to_ack(chan->mScalingKeys[j].mValue);
				}
			}

			if(chan->mNumRotationKeys > 0)
			{
				rot = ai_to_ack(chan->mRotationKeys[0].mValue);
				for(uint j = 1; j < chan->mNumRotationKeys; j++) {
					if(chan->mRotationKeys[j].mTime > frameTime) break;
					rot = ai_to_ack(chan->mRotationKeys[j].mValue);
				}
			}

			MATRIX transform;
			mat_id(&transform);
			mat_scale(&transform, &scale);
			mat_rotate(&transform, &rot);
			mat_translate(&transform, &pos);

			bone.transform = transform;
		} catch(std::exception & ex) {
			engine_log("Bone %s not found!", chan->mNodeName.C_Str());
		}
	}

	engine_log("Animate %s @ %f / %f", name, frameTime, anim->mDuration);
}

extern "C" MODEL * load_bonestructure(char const * file)
{
	unsigned int flags =
		  aiProcess_JoinIdenticalVertices
		| aiProcess_MakeLeftHanded
		| aiProcess_CalcTangentSpace
	    | aiProcess_GenUVCoords // required for tangent space :(
		| aiProcess_Triangulate
		| aiProcess_GenNormals
		| aiProcess_LimitBoneWeights
		| aiProcess_ValidateDataStructure
		| aiProcess_ImproveCacheLocality
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_OptimizeMeshes
		| aiProcess_OptimizeGraph
		| aiProcess_FlipUVs
		;

	scene = importer.ReadFile(file, flags);
	if(!scene) {
		engine_log("Failed to load: %s", importer.GetErrorString());
		return nullptr;
	}

	for(uint i = 0; i < scene->mNumAnimations; i++)
	{
		auto anim = scene->mAnimations[i];
		engine_log("Animation %d: %s", i, anim->mName.C_Str());
		engine_log("\tDuration: %f ÷ %f", anim->mDuration, anim->mTicksPerSecond);
		for(uint j = 0; j < anim->mNumChannels; j++)
		{
			// auto chan = anim->mChannels[j];
			// engine_log("\tChannel[%d]: %s", j, chan->mNodeName.C_Str());
		}
	}

	int boneCount = 0;
	{
		std::stack<aiNode const *> stack;
		stack.push(scene->mRootNode);
		while(stack.size() > 0)
		{
			++boneCount;
			aiNode const * top = stack.top();
			stack.pop();
			for(uint i = 0; i < top->mNumChildren; i++) {
				stack.push(top->mChildren[i]);
			}
		}
	}

	MODEL * model = model_create(scene->mNumMeshes, boneCount, scene->mNumAnimations);

	uint8_t counter = 0;
	translateNodes(model, scene->mRootNode, 0, counter);
	assert(counter == boneCount);

	for(uint index = 0; index < scene->mNumMeshes; index++)
	{
		auto mesh = scene->mMeshes[index];
		engine_log("Mesh: %s", mesh->mName.C_Str());

		model->meshes[index] = mesh_create(GL_TRIANGLES, NULL, NULL);
		model->meshes[index]->indexBuffer = buffer_create(INDEXBUFFER);
		model->meshes[index]->vertexBuffer = buffer_create(VERTEXBUFFER);

		buffer_set(model->meshes[index]->vertexBuffer, sizeof(VERTEX) * mesh->mNumVertices, NULL);
		buffer_set(model->meshes[index]->indexBuffer, sizeof(INDEX) * 3 * mesh->mNumFaces, NULL);

		{
			VERTEX * vertices = (VERTEX*)buffer_map(model->meshes[index]->vertexBuffer, WRITEONLY);

			memset(vertices, 0, model->meshes[index]->vertexBuffer->size);

			// Nontrivial init
			for(uint j = 0; j < mesh->mNumVertices; j++)
			{
				vertices[j].color = COLOR_WHITE;
				vertices[j].texcoord0 = (UVCOORD){0,0};
				vertices[j].texcoord1 = (UVCOORD){0,0};
				vertices[j].bones = (UBYTE4){ 0, 0, 0, 0 };
				vertices[j].boneWeights = (UBYTE4){255,0,0,0};

				vertices[j].position = ai_to_ack(mesh->mVertices[j]);
				vertices[j].normal = ai_to_ack(mesh->mNormals[j]);
				vertices[j].tangent = ai_to_ack(mesh->mTangents[j]);
			}

			if(mesh->HasTextureCoords(0))
			{
				for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
					auto const & vec = mesh->mTextureCoords[0][i];
					auto & dst = vertices[i];
					dst.texcoord0.u = vec.x;
					dst.texcoord0.v = vec.y;
				}
			}
			if(mesh->HasTextureCoords(1))
			{
				for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
					auto const & vec = mesh->mTextureCoords[1][i];
					auto & dst = vertices[i];
					dst.texcoord1.u = vec.x;
					dst.texcoord1.v = vec.y;
				}
			}
			if(mesh->HasVertexColors(0))
			{
				for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
					auto const & vec = mesh->mColors[0][i];
					auto & dst = vertices[i];
					dst.color.red = vec.r;
					dst.color.green = vec.g;
					dst.color.blue = vec.b;
					dst.color.alpha = vec.a;
				}
			}

			if(mesh->HasBones())
			{
				struct bonevert
				{
					int count = 0;
					uint8_t nodes[4] = { 0, 0, 0, 0 };
					float weights[4] = { 1, 0, 0, 0 };

					void insert(uint8_t id, float weight)
					{
						assert(this->count < 4);
						this->nodes[this->count] = id;
						this->weights[this->count] = weight;
						this->count++;
					}
				};

				std::vector<bonevert> buckets(mesh->mNumVertices);

				for(uint j = 0; j < mesh->mNumBones; j++)
				{
					auto bone = mesh->mBones[j];
					int index = -1;
					for(int k = 0; k < model->boneCount; k++)
					{
						if(strcmp(model->bones[k].name, bone->mName.C_Str()) == 0)
						{
							assert(index == -1);
							index = k;
							ai_to_ack(
								&model->bones[k].bindToBoneTransform,
								bone->mOffsetMatrix);
						}
					}
					assert(index >= 0);

					for(uint k = 0; k < bone->mNumWeights; k++)
					{
						buckets[bone->mWeights[k].mVertexId].insert(
							index,
							bone->mWeights[k].mWeight);
					}
				}

				for(uint j = 0; j < mesh->mNumVertices; j++)
				{
					auto & buck = buckets[j];
					vertices[j].bones = {
					    buck.nodes[0],
					    buck.nodes[1],
					    buck.nodes[2],
					    buck.nodes[3],
					};
					vertices[j].boneWeights = {
						(uint8_t)clamp(255 * buck.weights[0], 0, 255),
					    (uint8_t)clamp(255 * buck.weights[1], 0, 255),
					    (uint8_t)clamp(255 * buck.weights[2], 0, 255),
					    (uint8_t)clamp(255 * buck.weights[3], 0, 255),
					};
				}
			}

			buffer_unmap(model->meshes[index]->vertexBuffer);
		}
		{
			INDEX * indices = (INDEX*)buffer_map(model->meshes[index]->indexBuffer, WRITEONLY);
			memset(indices, 0, model->meshes[index]->indexBuffer->size);

			for(uint j = 0; j < mesh->mNumFaces; j++)
			{
				auto face = mesh->mFaces[j];
				assert(face.mNumIndices == 3);

				*indices++ = face.mIndices[0];
				*indices++ = face.mIndices[1];
				*indices++ = face.mIndices[2];
			}

			buffer_unmap(model->meshes[index]->indexBuffer);
		}
	}

	if(scene->HasMaterials())
	{
		std::vector<MATERIAL*> materials(scene->mNumMaterials);
		for(unsigned int idx = 0; idx < scene->mNumMaterials; idx++)
		{
			materials[idx] = mtl_create();
			MATERIAL & dst = *materials[idx];
			aiMaterial const & src = *scene->mMaterials[idx];
			engine_log("material %d");

			memset(&dst, 0, sizeof(MATERIAL));
			dst.albedo = (COLOR){1,1,1,1};
			dst.roughness = 1.0;

			aiColor4D cDiffuse, cEmissive;

			if(src.Get(AI_MATKEY_COLOR_DIFFUSE, cDiffuse) == aiReturn_SUCCESS) {
				dst.albedo.red = cDiffuse.r;
				dst.albedo.green = cDiffuse.g;
				dst.albedo.blue = cDiffuse.b;
				dst.albedo.alpha = cDiffuse.a;
			}

			if(src.Get(AI_MATKEY_COLOR_EMISSIVE, cEmissive) == aiReturn_SUCCESS) {
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
				dst.roughness = 1.0 - fShininess;
			}

			aiString path;
			if(src.GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS) {
				engine_log("Texture: %s", path.C_Str());

				if(path.data[0] == '*') {
					if(!scene->HasTextures()) {
						engine_log("Model '%s' wants to use internal texture, but has none!", file);
					} else {
						int index = atoi(path.data + 1);
						aiTexture * tex = scene->mTextures[index];
						if(tex->achFormatHint != nullptr) {
							engine_log("Texture format hint: '%s'", tex->achFormatHint);
						}
						dst.albedoTexture = bmap_create(GL_TEXTURE_2D, GL_RGBA);
						bmap_set(
							dst.albedoTexture,
							tex->mWidth,
							tex->mHeight,
							GL_BGRA,
							GL_UNSIGNED_BYTE,
							tex->pcData);
					}
				} else {
					dst.albedoTexture = bmap_load(path.C_Str());
				}

				if(dst.albedoTexture == nullptr) {
					engine_seterror(ERR_FILESYSTEM, "Could not load the texture %s", path.C_Str());
				}
			}
		}
		for(uint i = 0; i < scene->mNumMeshes; i++)
		{
			model->materials[i] = materials[scene->mMeshes[i]->mMaterialIndex];
		}
	}

	// printNodes(scene->mRootNode);

	return model;
}
