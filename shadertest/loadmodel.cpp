#include <acknext.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <string.h>
#include <stdlib.h>
#include <stack>
#include <vector>

char buf[256];

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

void printNodes(aiNode const * node, char const * indent = "")
{
	char buffer[256];
	strcpy(buffer, "");
	if(node->mNumMeshes > 0)
	{
		strcpy(buffer, "(");
		for(int i = 0; i < node->mNumMeshes; i++) {
			if(i > 0) strcat(buffer,", ");
			std::sprintf(buf, "%d", i);
			strcat(buffer, buf);
		}
		strcat(buffer, ")");
	}

	engine_log("%s%s%s", indent, node->mName.C_Str(), buffer);

	MATRIX nodeTransform;
	ai_to_ack(&nodeTransform, node->mTransformation);

	/*
	engine_log("trafo:\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f",
		nodeTransform.fields[0][0], nodeTransform.fields[0][1], nodeTransform.fields[0][2], nodeTransform.fields[0][3],
		nodeTransform.fields[1][0], nodeTransform.fields[1][1], nodeTransform.fields[1][2], nodeTransform.fields[1][3],
		nodeTransform.fields[2][0], nodeTransform.fields[2][1], nodeTransform.fields[2][2], nodeTransform.fields[2][3],
		nodeTransform.fields[3][0], nodeTransform.fields[3][1], nodeTransform.fields[3][2], nodeTransform.fields[3][3]);
	*/
	VECTOR4 vec = { 0, 0, 0, 1 };
	vec4_transform(&vec, &nodeTransform);

	engine_log("%s%f %f %f %f", indent, vec.x, vec.y, vec.z, vec.w);

	strcpy(buffer, indent);
	strcat(buffer, "  ");
	if(node->mNumChildren > 0 && node->mChildren)
	{
		for(int i = 0; i < node->mNumChildren; i++)
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
	// TODO: Copy bone.bindToBoneTransform from bone def
	for(int i = 0; i < node->mNumChildren; i++) {
		translateNodes(target, node->mChildren[i], id, index);
	}
}

extern "C" MODEL * load_bonestructure(char const * file)
{
	using namespace Assimp;
	Importer importer;

	auto fags = aiProcess_CalcTangentSpace
	        | aiProcess_JoinIdenticalVertices
	        | aiProcess_MakeLeftHanded
	        | aiProcess_Triangulate
	        | aiProcess_GenNormals
	        | aiProcess_LimitBoneWeights
	        | aiProcess_ValidateDataStructure
	        | aiProcess_FindInstances
	        | aiProcess_OptimizeGraph
	        | aiProcess_OptimizeMeshes
			;

	aiScene const * scene = importer.ReadFile(file,fags);
	if(!scene) {
		engine_log("Failed to load: %s", importer.GetErrorString());
		return nullptr;
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
			for(int i = 0; i < top->mNumChildren; i++) {
				stack.push(top->mChildren[i]);
			}
		}
	}

	MODEL * model = model_create(scene->mNumMeshes, scene->mNumMaterials, boneCount, scene->mNumAnimations);

	uint8_t counter = 0;
	translateNodes(model, scene->mRootNode, 0, counter);
	assert(counter == boneCount);

	for(uint i = 0; i < scene->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[i];
		engine_log("Mesh: %s", mesh->mName.C_Str());

		model->meshes[i] = (MESH*)malloc(sizeof(MESH));
		model->meshes[i]->material = NULL;
		model->meshes[i]->indexBuffer = buffer_create(INDEXBUFFER);
		model->meshes[i]->vertexBuffer = buffer_create(VERTEXBUFFER);

		buffer_set(model->meshes[i]->vertexBuffer, sizeof(VERTEX) * mesh->mNumVertices, NULL);
		buffer_set(model->meshes[i]->indexBuffer, sizeof(INDEX) * 3 * mesh->mNumFaces, NULL);

		{
			VERTEX * vertices = (VERTEX*)buffer_map(model->meshes[i]->vertexBuffer, WRITEONLY);

			memset(vertices, 0, model->meshes[i]->vertexBuffer->size);

			// Nontrivial init
			for(uint j = 0; j < mesh->mNumVertices; j++)
			{
				vertices[j].boneWeights = { 255, 0, 0, 0 };
				vertices[j].color = COLOR_WHITE;


				vertices[j].position = ai_to_ack(mesh->mVertices[j]);
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
				memset(buckets.data(), 0, sizeof(int) * buckets.size());
				for(uint j = 0; j < mesh->mNumBones; j++)
				{
					auto bone = mesh->mBones[j];
					int index = -1;
					for(uint k = 0; k < model->boneCount; k++)
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
			}

			buffer_unmap(model->meshes[i]->vertexBuffer);
		}
		{
			INDEX * indices = (INDEX*)buffer_map(model->meshes[i]->indexBuffer, WRITEONLY);
			memset(indices, 0, model->meshes[i]->indexBuffer->size);

			for(uint j = 0; j < mesh->mNumFaces; j++)
			{
				auto face = mesh->mFaces[j];
				assert(face.mNumIndices == 3);

				*indices++ = face.mIndices[0];
				*indices++ = face.mIndices[1];
				*indices++ = face.mIndices[2];
			}

			buffer_unmap(model->meshes[i]->indexBuffer);
		}
	}

	// printNodes(scene->mRootNode);

	return model;
}
