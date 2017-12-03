#include "material.hpp"

Material::Material(bool userCreated) :
    EngineObject<MATERIAL>(),
    userCreated(userCreated)
{
	api().albedo = (COLOR) { 1, 1, 1, 1 };
}

Material::~Material()
{

}

ACKNEXT_API_BLOCK
{
	MATERIAL * mtl_create()
	{
		return demote(new Material(true));
	}

	void mtl_remove(MATERIAL * _mtl)
	{
		Material * mtl = promote<Material>(_mtl);
		if(mtl && mtl->userCreated) {
			delete mtl;
		}
	}
}
