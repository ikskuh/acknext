#include "engine.h"
#include "level-detail.h"
#include "entity-detail.h"

ENTITYdetail::ENTITYdetail(ENTITY * ent) : ent(ent), model(nullptr)
{

}

ENTITYdetail::~ENTITYdetail()
{

}

ACKFUN ENTITY * ent_create(char const * source, VECTOR const * position, void (*entmain)())
{
    // Implement model assignment.
    (void)source;

	Model * model = nullptr;
	if(source != nullptr) {
		model = model_get(source);
		if(model == nullptr) {
			engine_log("Failed to load model %s", source);
			return nullptr;
		}
	}

    ENTITY * ent = new ENTITY;
    ent->position = *position;
    ent->parent = NULL;
    ent->flags = NONE;
	ent->_detail = new ENTITYdetail(ent);
	ent->_detail->model = model;

	if(entmain != NULL) {
		TASK * hMain = task_start(entmain, ent);

		scheduler_setvar(hMain, &me, &ent, sizeof(me));
		scheduler_setvar(hMain, &my, &ent, sizeof(my));
	}

	ent_attach(ent, world);

    return ent;
}

ACKFUN void ent_attach(ENTITY * ent, LEVEL * level)
{
    if(ent == nullptr || level == nullptr) {
        return;
    }
    level->_detail->entities.insert(ent);
}

ACKFUN void ent_detach(ENTITY * ent, LEVEL * level)
{
    if(ent == nullptr || level == nullptr) {
        return;
    }
    auto pos = level->_detail->entities.find(ent);
    if(pos != level->_detail->entities.end()) {
        level->_detail->entities.erase(pos);
    }
}

ACKFUN void ent_remove(ENTITY * ent)
{
	if(ent == nullptr) {
		return;
	}
	delete ent->_detail;
	delete ent;
}
