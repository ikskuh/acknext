#ifndef MESH_HPP
#define MESH_HPP

#include "engine.hpp"

class Mesh :
	public EngineObject<MESH>
{
public:
	explicit Mesh(GLenum primitiveType = GL_TRIANGLES);
	NOCOPY(Mesh);
	~Mesh();
};

#endif // MESH_HPP
