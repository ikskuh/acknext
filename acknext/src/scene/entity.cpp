#include "entity.hpp"
#include "../events/event.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../graphics/scene/ackglm.hpp"

Entity * Entity::first = nullptr;
Entity * Entity::last = nullptr;

Entity::Entity() :
    EngineObject<ENTITY>(),
    previous(last),
    next(nullptr),
    hullProvider(nullptr)
{
	// insert
	if(Entity::first == nullptr) {
		Entity::first = Entity::last = this;
	} else {
		Entity::last->next = this;
		Entity::last = this;
	}

	api().removed = demote(new Event(false));

	vec_fill(&api().scale, 1);
	quat_id(&api().rotation);
	api().flags |= VISIBLE;
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
		if(fileName) {
			ent->model = model_get(fileName);
			if(ent->model == nullptr) {
				ent_remove(ent);
				engine_seterror(ERR_FILESYSTEM, "The model file %s could not be found!", fileName);
				return nullptr;
			}
		}
		if(position) ent->position = *position;
		if(action) {
			// TODO: Associate the created task with
			// the entity and kill it on entity destruction
			// :(
			task_defer(action, ent);
		}

		if(ent->model) {
			// Go into initial pose
			ent_posereset(ent);

			// Update the collision hull
			ent_updatehull(ent);
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

	ENTITY * ent_next(ENTITY const * _ent)
	{
		Entity const * ent = promote<Entity>(_ent);
		if(ent != nullptr) {
			return demote(ent->next);
		} else {
			return demote(Entity::first);
		}
	}

	void ent_updatehull(ENTITY * ent)
	{
		ARG_NOTNULL(ent,);
		Entity * subent = promote<Entity>(ent);

		// yay, we're already done!
		if(subent->hullProvider == ent->model)
			return;

		if(ent->mainCollider != nullptr)
			hull_remove(ent->mainCollider);

		if(ent->model && ent->model->createCollider)
		{
			dGeomID newHull = (ent->model->createCollider)(collision_space, ent->model);
			if(newHull)
				ent->mainCollider = hull_createFromExisting(ent, newHull);
			else
				ent->mainCollider = nullptr;
		}
		else
		{
			ent->mainCollider = nullptr;
		}

		subent->hullProvider = ent->model;
	}

	// Resets the entities pose to the entities models default pose
	ACKFUN void ent_posereset(ENTITY * ent)
	{
		ARG_NOTNULL(ent,);
		if(ent->model)
		{
			for(int i = 0; i < ACKNEXT_MAX_BONES; i++)
			{
				glm::mat4 mat = ack_to_glm(ent->model->bones[i].transform);

				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 translation;
				glm::vec3 skew;
				glm::vec4 perspective;

				glm::decompose(mat, scale, rotation, translation, skew, perspective);

				glm_to_ack(&ent->pose[i].position, translation);
				glm_to_ack(&ent->pose[i].rotation, glm::conjugate(rotation));
				glm_to_ack(&ent->pose[i].scale, scale);
			}
		} else {
			static const FRAME id =
			{
				time:     0,
			    position: { 0, 0, 0 },
			    rotation: { 0, 0, 0, 1},
			    scale:    { 1, 1, 1 },
			};
			for(int i = 0; i < ACKNEXT_MAX_BONES; i++)
				ent->pose[i] = id;
		}
	}

	ACKFUN void ent_animate(ENTITY * ent, char const * animation, double progress)
	{
		// TODO: Add interpolation
		ARG_NOTNULL(ent,);
		ARG_NOTNULL(animation,);

		MODEL * model = ent->model;
		if(!model) {
			engine_seterror(ERR_INVALIDOPERATION, "Cannot animate entity without model!");
			return;
		}

		ANIMATION const * anim = nullptr;
		for(int i = 0; i < model->animationCount; i++)
		{
			if(strcmp(model->animations[i]->name, animation) == 0) {
				anim = model->animations[i];
				break;
			}
		}
		if(!anim) {
			engine_seterror(ERR_INVALIDOPERATION, "The animation '%s' could not be found!", animation);
			return;
		}

		if(anim->duration > 0 && (anim->flags & LOOPED))
			progress = fmod(progress, anim->duration);

		for(int i = 0; i < anim->channelCount; i++)
		{
			CHANNEL const * chan = anim->channels[i];
			assert(chan->frameCount > 0);

			FRAME frame = chan->frames[0];
			for(int j = 1; j < chan->frameCount; j++) {
				if(chan->frames[j].time > progress) break;
				frame = chan->frames[j];
			}
			ent->pose[chan->targetBone] = frame;
		}
	}
}
