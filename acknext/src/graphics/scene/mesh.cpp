#include "mesh.hpp"
#include <float.h>


Mesh::Mesh(GLenum primitiveType) :
    EngineObject<MESH>()
{
	api().primitiveType = primitiveType;
	api().lodMask = 0xFFFFUL;
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

	void mesh_updateBoundingBox(MESH * mesh)
	{
		if(mesh && mesh->vertexBuffer)
		{
			AABB & aabb = mesh->boundingBox;
			aabb.maximum = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
			aabb.minimum = {  FLT_MAX,  FLT_MAX,  FLT_MAX };

			size_t cnt = mesh->vertexBuffer->size / sizeof(VERTEX);
			VERTEX * list = (VERTEX*)buffer_map(mesh->vertexBuffer, READONLY);
			for(size_t i = 0; i < cnt; i++)
			{
				aabb.maximum.x = maxv(aabb.maximum.x, list[i].position.x);
				aabb.maximum.y = maxv(aabb.maximum.y, list[i].position.y);
				aabb.maximum.z = maxv(aabb.maximum.z, list[i].position.z);

				aabb.minimum.x = minv(aabb.minimum.x, list[i].position.x);
				aabb.minimum.y = minv(aabb.minimum.y, list[i].position.y);
				aabb.minimum.z = minv(aabb.minimum.z, list[i].position.z);
			}
			buffer_unmap(mesh->vertexBuffer);
		}
	}
}
