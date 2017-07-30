#include "engine.h"

#include <list>
#include <set>
#include <memory>

#include "level-detail.h"
#include <json.hpp>

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
