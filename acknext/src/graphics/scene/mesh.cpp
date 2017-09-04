#include "mesh.hpp"

Mesh::Mesh(GLenum primitiveType) :
    EngineObject<MESH>()
{
	api().primitiveType = primitiveType;
}

Mesh::~Mesh()
{

}

ACKNEXT_API_BLOCK
{
	MESH * mesh_create(GLenum primitiveType, BUFFER * vertexBuffer, BUFFER * indexBuffer)
	{
		MESH * mesh = demote(new Mesh(primitiveType));
		mesh->vertexBuffer = vertexBuffer;
		mesh->indexBuffer = indexBuffer;
		return mesh;
	}

	void mesh_remove(MESH * mesh)
	{
		Mesh * m = promote<Mesh>(mesh);
		if(m)
			delete m;
	}
}
