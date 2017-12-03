#include "light.hpp"

Light * Light::first = nullptr;
Light * Light::last = nullptr;


Light::Light(LIGHTTYPE type) :
    EngineObject<LIGHT>(),
    previous(last),
    next(nullptr)
{
	// insert
	if(Light::first == nullptr) {
		Light::first = Light::last = this;
	} else {
		Light::last->next = this;
		Light::last = this;
	}

	api().type = type;
	api().color = (COLOR){1,1,1,1};
	api().intensity = 10;
	api().arc = 45;
}

Light::~Light()
{
	if(this->previous) {
		this->previous->next = this->next;
	}
	if(this->next) {
		this->next->previous = this->previous;
	}
	if(Light::first == this) {
		Light::first = this->next;
	}
	if(Light::last == this) {
		Light::last = this->last;
	}
}

ACKNEXT_API_BLOCK
{
	LIGHT * light_create(LIGHTTYPE type)
	{
		return demote(new Light(type));
	}

	void light_remove(LIGHT * light)
	{
		Light * l = promote<Light>(light);
		if(l) delete l;
	}

	LIGHT * light_next(LIGHT const * _light)
	{
		Light const * light = promote<Light>(_light);
		if(light != nullptr) {
			return demote(light->next);
		} else {
			return demote(Light::first);
		}
	}
}
