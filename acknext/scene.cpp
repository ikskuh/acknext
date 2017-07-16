#include "engine.h"

#include <list>
#include <set>
#include <memory>

#include <json.hpp>

#include "level-detail.h"

struct LEVELdeleter
{
    void operator ()(LEVEL *level)
    {
        delete level->_detail;
        delete level;
    }
};

using LEVELuniq = std::unique_ptr<LEVEL, LEVELdeleter>;
static std::list<LEVELuniq> levels;

ACKFUN LEVEL * level_create()
{
    LEVEL * level = new LEVEL;

    level->filename = nullptr;
    level->flags = NONE;
    level->_detail = new LEVELdetail(level);

    levels.emplace_back(level);

    return level;
}

ACKFUN LEVEL * level_load(char const * filename)
{
    nlohmann::json json(filename);

    // TODO: Implement level loader.

    throw "Not implemented yet!";
}

ACKFUN void level_remove(LEVEL * level)
{
    ::levels.remove_if([level](LEVELuniq & ptr) {
        return ptr.get() == level;
    });
}

ACKFUN ENTITY * ent_create(char const * source, VECTOR position, void (*entmain)())
{
    // Implement model assignment.
    (void)source;

    ENTITY * ent = new ENTITY;
    ent->position = position;
    ent->parent = NULL;
    ent->flags = NONE;

	if(entmain != NULL) {
		TASK * hMain = task_start(entmain, ent);
		(void)hMain;
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
