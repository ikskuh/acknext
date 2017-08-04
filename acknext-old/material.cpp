#include "engine.h"

ACKFUN MATERIAL * mtl_create()
{
	MATERIAL * mtl = new MATERIAL;
	memset(mtl, 0, sizeof(MATERIAL));
	mtl->color = (COLOR){ 1, 1, 1, 1 };
	mtl->roughness = 1.0f;
	return mtl;
}

ACKFUN void mtl_remove(MATERIAL * mtl)
{
	if(mtl != nullptr) {
		delete mtl;
	}
}
