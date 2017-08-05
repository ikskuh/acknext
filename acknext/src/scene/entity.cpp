#include "entity.hpp"
#include "../events/event.hpp"

Entity * Entity::first = nullptr;
Entity * Entity::last = nullptr;

Entity::Entity() :
    previous(last),
    next(nullptr)
{
	// insert
	if(Entity::first == nullptr) {
		Entity::first = Entity::last = this;
	} else {
		Entity::last->next = this;
		Entity::last = this;
	}

	api().removed = demote(new Event(false));
}

Entity::~Entity()
{
	event_invoke(api().removed, demote(this));
	if(this->previous) {
		this->previous->next = this->next;
	}
	if(this->next) {
		this->next->previous = this->previous;
	}
	if(Entity::first == this) {
		Entity::first = this->next;
	}
	if(Entity::last == this) {
		Entity::last = this->last;
	}
}

#include <engine.hpp>

ACKNEXT_API_BLOCK
{
	ENTITY * ent_create(
		char const * fileName,
		VECTOR * const position,
		ENTRYPOINT action)
	{
		ENTITY * ent = demote(new Entity());
		if(fileName) ent->model = model_get(fileName);
		if(position) ent->position = *position;
		if(action) {
			// TODO: Associate the created task with
			// the entity and kill it on entity destruction
			task_start(action, ent);
		}
		return ent;
	}

	void ent_remove(ENTITY * _ent)
	{
		Entity * ent = promote<Entity>(_ent);
		if(ent) {
			delete ent;
		}
	}

	ENTITY * ent_next(ENTITY * _ent)
	{
		Entity * ent = promote<Entity>(_ent);
		if(ent != nullptr) {
			return demote(ent->next);
		} else {
			return demote(Entity::first);
		}
	}
}
