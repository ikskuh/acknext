#include "model.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <stack>
#include <assert.h>
#include <libgen.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

static std::unordered_map<std::string, MODEL*> modelCache;

static inline VECTOR ass_to_ack(const aiVector3D & vec)
{
	return (VECTOR){ vec.x, vec.y, vec.z };
}

class AckIoStream : public Assimp::IOStream
{
	ACKFILE * file;
public:
	AckIoStream(ACKFILE * file) : file(file) { }
	~AckIoStream() { file_close(file); }

	size_t Read( void* pvBuffer, size_t pSize, size_t pCount)
	{
		return file_read(file, pvBuffer, pSize * pCount);
	}

	size_t Write( const void* pvBuffer, size_t pSize, size_t pCount)
	{
		return file_write(file, pvBuffer, pSize * pCount);
	}

	aiReturn Seek( size_t pOffset, aiOrigin pOrigin)
	{
		if(pOrigin != aiOrigin_SET) {
			return aiReturn_FAILURE;
		}
		file_seek(file, pOffset);
		return aiReturn_SUCCESS;
	}

	size_t Tell() const
	{
		return file_tell(file);
	}

	size_t FileSize() const
	{
		return file_size(file);
	}

	void Flush ()
	{
		file_flush(file);
	}
};

class AckIoSystem : public Assimp::IOSystem
{
public:
	virtual bool Exists(const char *pFile) const override
	{
		return !!PHYSFS_exists(pFile);
	}

	virtual char getOsSeparator() const override { return '/'; }

	virtual Assimp::IOStream * Open(const char *pFile, const char *pMode) override
	{
		ACKFILE * file = nullptr;
		if(strchr(pMode, 'r')) {
			file = file_open_read(pFile);
		}
		else if(strchr(pMode, 'w')) {
			file = file_open_write(pFile);
		}
		else if(strchr(pMode, 'a')) {
			file = file_open_append(pFile);
		}
		else {
			abort();
		}
		if(file == nullptr) {
			return nullptr;
		}
		return new AckIoStream(file);
	}

	virtual void Close(Assimp::IOStream *pFile)
	{
		delete pFile;
	}
};

static AckIoSystem iosystem;

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

ACKNEXT_API_BLOCK
{
	MODEL * model_create(int numMeshes, int numBones, int numAnimations)
	{
		Model * model = new Model();
		MODEL & api = model->api();

		api.meshCount = maxv(numMeshes, 1);
		api.boneCount = maxv(numBones, 1);
		api.animationCount = maxv(numAnimations, 0);

		api.meshes = (MESH**)malloc(sizeof(MESH*)*api.meshCount);
		api.materials = (MATERIAL**)malloc(sizeof(MATERIAL*)*api.meshCount);
		api.animations = (ANIMATION**)malloc(sizeof(ANIMATION*)*api.animationCount);

		memset(api.meshes, 0, sizeof(MESH*)*api.meshCount);
		memset(api.materials, 0, sizeof(MESH*)*api.meshCount);
		memset(api.animations, 0, sizeof(MESH*)*api.animationCount);

		for(int i = 0; i < ACKNEXT_MAX_BONES; i++) {
			api.bones[i].parent = 0;
			strcpy(api.bones[i].name, "");
			mat_id(&api.bones[i].transform);
			mat_id(&api.bones[i].bindToBoneTransform);
		}

		return demote(model);
	}


	/*
	MODEL * model_load(const char *fileName)
	{
		engine_seterror(ERR_INVALIDOPERATION, "model_load not implemented yet!");
		return nullptr;
	}
	*/

	MODEL * model_load(const char *fileName)
	{
		using namespace Assimp;

		if(fileName == nullptr) {
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

		Importer importer;
		importer.SetIOHandler(new AckIoSystem);
		aiScene const * scene = importer.ReadFile(fileName, flags);
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
							engine_log("Model '%s' wants to use internal texture, but has none!", fileName);
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

						char buffer[PATH_MAX];
						strcpy(buffer, path.C_Str());
						if(buffer[0] != '/') {
							strcpy(buffer, fileName);
							dirname(buffer);
							strcat(buffer, "/");
							strcat(buffer, path.C_Str());
						}
						engine_log("abspath: %s", buffer);

						dst.albedoTexture = bmap_load(buffer);
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
	/*

		unsigned int flags =
			  aiProcess_JoinIdenticalVertices
			| aiProcess_CalcTangentSpace
		    | aiProcess_GenUVCoords // required for tangent space :(
			| aiProcess_Triangulate
			| aiProcess_GenNormals
			| aiProcess_PreTransformVertices // REMOVE THIS LATER!
			| aiProcess_LimitBoneWeights
			| aiProcess_ValidateDataStructure
			| aiProcess_ImproveCacheLocality
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_OptimizeMeshes
			| aiProcess_OptimizeGraph
			| aiProcess_FlipUVs
			;

		aiScene const * scene = importer.ReadFile(fileName,	flags);

		if(scene == nullptr) {
			engine_seterror(ERR_INVALIDOPERATION, importer.GetErrorString());
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
					dst.roughness = fShininess;
				}

				aiString path;
				if(src.GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS) {
					engine_log("Texture: %s", path.C_Str());

					if(path.data[0] == '*') {
						if(!scene->HasTextures()) {
							engine_log("Model '%s' wants to use internal texture, but has none!", fileName);
						} else {
							int index = atoi(path.data + 1);
							aiTexture * tex = scene->mTextures[index];
							if(tex->achFormatHint != nullptr) {
								engine_log("Texture format hint: '%s'", tex->achFormatHint);
							}
							dst.colorTexture = bmap_create(GL_TEXTURE_2D, GL_RGBA);
							bmap_set(
								dst.colorTexture,
								tex->mWidth,
								tex->mHeight,
								GL_BGRA,
								GL_UNSIGNED_BYTE,
								tex->pcData);
						}
					} else {
						dst.colorTexture = bmap_load(path.C_Str());
					}

					if(dst.colorTexture == nullptr) {
						engine_seterror(ERR_FILESYSTEM, "Could not load the texture %s", path.C_Str());
					}
				}
			}
		}

		model->meshes.resize(scene->mNumMeshes);
		for(unsigned int idx = 0; idx < scene->mNumMeshes; idx++)
		{
			MESH * dst = &model->meshes[idx];
			aiMesh * src = scene->mMeshes[idx];

			if(src->mPrimitiveTypes & ~aiPrimitiveType_TRIANGLE) {
				engine_log("Invalid primitive type detected: %x", src->mPrimitiveTypes);
				abort();
			}

			if(!src->HasNormals()) {
				engine_log("Mesh is missing normals!");
				abort();
			}

			if(!src->HasTangentsAndBitangents()) {
				engine_log("Mesh is missing normals (tangent/bitangent)q!");
				abort();
			}

			dst->indexBuffer = buffer_create(INDEXBUFFER);
			dst->vertexBuffer = buffer_create(VERTEXBUFFER);
			dst->material = &model->materials[src->mMaterialIndex];

			{
				std::vector<VERTEX> vertices(src->mNumVertices);
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

					dst.texcoord0 = (UVCOORD){0,0};
					dst.texcoord1 = (UVCOORD){0,0};
					dst.bones = (UBYTE4){ 0, 0, 0, 0 };
					dst.boneWeights = (UBYTE4){1,0,0,0};
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
					vertices.data());
			}

			{
				std::vector<INDEX> indices(3 * src->mNumFaces);
				for(unsigned int i = 0; i < src->mNumFaces; i++) {
					aiFace const & face = src->mFaces[i];
					for(int j = 0; j < 3; j++) {
						indices[3 * i + j] = face.mIndices[j];
					}
				}
				buffer_set(
					dst->indexBuffer,
					3 * src->mNumFaces * sizeof(INDEX),
					indices.data());
			}
		}

		if(scene->mRootNode->mNumChildren > 0) {
			engine_log("Models with graph structure are not supported yet!");
			// TODO: Add node graph to model structure
			abort();
		}

		return demote(model);
	}
	*/

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
}
