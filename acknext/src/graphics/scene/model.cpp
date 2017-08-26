#include "model.hpp"

#include <string>
#include <unordered_map>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

static std::unordered_map<std::string, MODEL*> modelCache;

static VECTOR ass_to_ack(const aiVector3D & vec)
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
	for(MESH & mesh : meshes) {
		buffer_remove(mesh.indexBuffer);
		buffer_remove(mesh.vertexBuffer);
	}
	for(MATERIAL & mtl : materials) {
		bmap_remove(mtl.colorTexture);
		bmap_remove(mtl.emissionTexture);
		bmap_remove(mtl.attributeTexture);
	}
}

ACKNEXT_API_BLOCK
{
	MODEL * model_create(int numMeshes, int numMaterials, int numBones, int numAnimations)
	{
		Model * model = new Model();
		MODEL & api = model->api();

		api.meshCount = maxv(numMeshes, 1);
		api.materialCount = maxv(numMaterials, 1);
		api.boneCount = maxv(numBones, 1);
		api.animationCount = maxv(numAnimations, 1);

		api.meshes = (MESH**)malloc(sizeof(MESH*)*api.meshCount);
		api.animations = (ANIMATION**)malloc(sizeof(ANIMATION*)*api.animationCount);
		api.materials = (MATERIAL**)malloc(sizeof(MATERIAL*)*api.materialCount);

		return demote(model);
	}

	MODEL * model_load(const char *fileName)
	{
		using namespace Assimp;

		if(fileName == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "fileName must not be NULL!");
			return nullptr;
		}

		Importer importer;
		importer.SetIOHandler(new AckIoSystem);

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
							dst.colorTexture = bmap_create(TEX_2D);
							bmap_set(
								dst.colorTexture,
								tex->mWidth,
								tex->mHeight,
								FORMAT_BGRA8,
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
