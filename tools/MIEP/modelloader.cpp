#include "modelloader.hpp"
#include "mainwindow.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <stack>
#include <assert.h>
#include <libgen.h>

#include <QFileInfo>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

static inline VECTOR ass_to_ack(const aiVector3D & vec)
{
	return (VECTOR){ vec.x, vec.y, vec.z };
}

static inline QUATERNION ass_to_ack(const aiQuaternion & vec)
{
	return (QUATERNION){ vec.x, vec.y, vec.z, vec.w };
}

static void ai_to_ack(MATRIX * dst, aiMatrix4x4 const & src)
{
	var array[4][4] = {
		{ src.a1, src.b1, src.c1, src.d1 },
	    { src.a2, src.b2, src.c2, src.d2 },
	    { src.a3, src.b3, src.c3, src.d3 },
	    { src.a4, src.b4, src.c4, src.d4 },
	};
	memcpy(dst->fields, array, sizeof(var) * 16);
}

static void putmat(char const * title, MATRIX const * mat)
{
	engine_log("%s\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f",
		title,
		mat->fields[0][0], mat->fields[0][1], mat->fields[0][2], mat->fields[0][3],
		mat->fields[1][0], mat->fields[1][1], mat->fields[1][2], mat->fields[1][3],
		mat->fields[2][0], mat->fields[2][1], mat->fields[2][2], mat->fields[2][3],
		mat->fields[3][0], mat->fields[3][1], mat->fields[3][2], mat->fields[3][3]);
}

static void printNodes(aiNode const * node, char const * indent = "")
{
	char buffer[256];
	strcpy(buffer, "");
	if(node->mNumMeshes > 0)
	{
		strcpy(buffer, "(");
		for(uint i = 0; i < node->mNumMeshes; i++) {
			if(i > 0) strcat(buffer,", ");
			char buf[64];
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

static void translateNodes(MODEL * target, aiNode const * node, uint8_t parent, uint8_t & index)
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

ModelLoader::ModelLoader(QObject *parent) : QObject(parent)
{

}

MODEL * ModelLoader::load(QString const & _fileName)
{
	MainWindow::makeCurrent();

	using namespace Assimp;

	if(_fileName.isNull()) {
		engine_seterror(ERR_INVALIDARGUMENT, "fileName must not be NULL!");
		return nullptr;
	}

	unsigned int flags =
		  aiProcess_JoinIdenticalVertices
		// | aiProcess_MakeLeftHanded // is this needed?
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

	auto fileName = _fileName.toUtf8().toStdString();

	Importer importer;
	// importer.SetIOHandler(new AckIoSystem);
	aiScene const * scene = importer.ReadFile(fileName, flags);
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

				vertices[j].position = ass_to_ack(mesh->mVertices[j]);
				vertices[j].normal = ass_to_ack(mesh->mNormals[j]);
				vertices[j].tangent = ass_to_ack(mesh->mTangents[j]);
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
		for(uint i = 0; i < scene->mNumMeshes; i++)
		{
			model->materials[i] = this->convertMaterial(
				scene->mMaterials[scene->mMeshes[i]->mMaterialIndex],
				scene,
				fileName);
		}
	}

	if(scene->HasAnimations())
	{
		assert(size_t(scene->mNumAnimations) == size_t(model->animationCount));
		for(uint i = 0; i < scene->mNumAnimations; i++)
		{
			auto const * src = scene->mAnimations[i];
			char name[256];
			strncpy(name, src->mName.C_Str(), 256);
			if(strlen(name) == 0) {
				sprintf(name, "Animation %d", i);
			}

			ANIMATION * dst = anim_create(name, src->mNumChannels);

			double timeScale = 1.0 / 25.0;
			if(src->mTicksPerSecond > 0) {
				timeScale = 1.0 / src->mTicksPerSecond;
			}

			dst->duration = timeScale * src->mDuration;

			for(uint j = 0; j < src->mNumChannels; j++)
			{
				auto * const chanSrc = src->mChannels[j];

				int frameCount = std::max(std::max(
					chanSrc->mNumPositionKeys,
					chanSrc->mNumRotationKeys),
					chanSrc->mNumScalingKeys);

				auto nodeName = chanSrc->mNodeName;

				CHANNEL * chanDst = chan_create(frameCount);

				chanDst->targetBone = -1;
				for(int i = 0; i < model->boneCount; i++) {
					if(strcmp(model->bones[i].name, nodeName.C_Str()) == 0) {
						chanDst->targetBone = i;
						break;
					}
				}
				assert(chanDst->targetBone >= 0 && chanDst->targetBone < model->boneCount);

				for(int i = 0; i < frameCount; i++)
				{
					if(chanSrc->mNumPositionKeys == uint(frameCount)) {
						chanDst->frames[i].time = timeScale * chanSrc->mPositionKeys[i].mTime;
					}
					else if(chanSrc->mNumRotationKeys == uint(frameCount)) {
						chanDst->frames[i].time = timeScale * chanSrc->mRotationKeys[i].mTime;
					}
					else if(chanSrc->mNumScalingKeys == uint(frameCount)) {
						chanDst->frames[i].time = timeScale * chanSrc->mScalingKeys[i].mTime;
					}
					else {
						assert(false);
					}
				}
				for(int i = 0; i < frameCount; i++)
				{
					FRAME & frame = chanDst->frames[i];
					frame.position = ass_to_ack(chanSrc->mPositionKeys[0].mValue);
					frame.scale = ass_to_ack(chanSrc->mScalingKeys[0].mValue);
					frame.rotation = ass_to_ack(chanSrc->mRotationKeys[0].mValue);

					// frame.position = (VECTOR){0,0,0};
					// frame.scale = (VECTOR){1,1,1};
					// frame.rotation = *quat_id(NULL);

					for(uint j = 1; j < chanSrc->mNumPositionKeys; j++) {
						if(timeScale * chanSrc->mPositionKeys[j].mTime < frame.time) {
							frame.position = ass_to_ack(chanSrc->mPositionKeys[j].mValue);
						}
					}
					for(uint j = 1; j < chanSrc->mNumRotationKeys; j++) {
						if(timeScale * chanSrc->mRotationKeys[j].mTime <= frame.time) {
							frame.rotation = ass_to_ack(chanSrc->mRotationKeys[j].mValue);
						}
					}

					for(uint j = 1; j < chanSrc->mNumScalingKeys; j++) {
						if(timeScale * chanSrc->mScalingKeys[j].mTime <= frame.time) {
							frame.scale = ass_to_ack(chanSrc->mScalingKeys[j].mValue);
						}
					}
				}
				dst->channels[j] = chanDst;
			}

			model->animations[i] = dst;
		}
	}

	return model;
}

MATERIAL * ModelLoader::convertMaterial(aiMaterial const * _src, aiScene const * scene, std::string const & referenceFileName)
{
	MATERIAL * _dst = mtl_create();

	aiMaterial const & src = *_src;
	MATERIAL & dst = *_dst;
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
				engine_log("Model '%s' wants to use internal texture, but has none!", referenceFileName.c_str());
			} else {
				int index = atoi(path.data + 1);
				aiTexture * tex = scene->mTextures[index];
				if(tex->achFormatHint != nullptr) {
					engine_log("Texture format hint: '%s'", tex->achFormatHint);
				}
				dst.albedoTexture = bmap_create(GL_TEXTURE_2D, GL_RGBA8);
				bmap_set(
					dst.albedoTexture,
					tex->mWidth,
					tex->mHeight,
					GL_BGRA,
					GL_UNSIGNED_BYTE,
					tex->pcData);

				const_cast<void*&>(dst.albedoTexture->pixels) = malloc(sizeof(aiTexel) * tex->mWidth * tex->mHeight);
				memcpy(dst.albedoTexture->pixels, tex->pcData, sizeof(aiTexel) * tex->mWidth * tex->mHeight);
			}
		} else {

			char buffer[PATH_MAX];
			strcpy(buffer, path.C_Str());
			if(buffer[0] != '/') {
				strcpy(buffer, referenceFileName.c_str());
				dirname(buffer);
				strcat(buffer, "/");
				strcat(buffer, path.C_Str());
			}
			engine_log("abspath: %s", buffer);

			if(QFileInfo(buffer).suffix() == "atx") {
				// Use ATX texture loader here
				assert(false);
			} else {
				dst.albedoTexture = bmap_load(buffer);
			}
		}

		if(dst.albedoTexture == nullptr) {
			engine_seterror(ERR_FILESYSTEM, "Could not load the texture %s", path.C_Str());
		}
	}
	return _dst;
}
