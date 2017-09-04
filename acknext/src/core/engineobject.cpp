#include "engineobject.hpp"
#include "engine.hpp"

#include <stdio.h>
#include <string.h>

Property::Property(GLenum type, void const * data)
{
	this->set(type, data);
}

void Property::set(GLenum type, void const * data)
{
	this->type = type;
	memcpy(&this->data, data, sizeof(this->data));
}

BaseEngineObject::BaseEngineObject() : magic(0xBADC0DED)
{

}

BaseEngineObject::~BaseEngineObject()
{

}

void BaseEngineObject::removeProperty(std::string const & name)
{
	this->properties.erase(name);
}

void BaseEngineObject::setProperty(std::string const & name, Property const & value)
{
	this->properties[name] = value;
}

Property const & BaseEngineObject::getProperty(std::string const & name) const
{
	return this->properties[name];
}

ACKNEXT_API_BLOCK
{
	void obj_setvar(void * obj, char const * name, GLenum type, void const * data)
	{
		Dummy * ptr = promote<Dummy>(reinterpret_cast<DUMMY*>(obj));
		if(ptr == nullptr) {
			engine_log("obj_setvar received NULL object!");
			return;
		}
		ptr->setProperty(name, Property(type, data));
	}

	void obj_listvar(void const * obj)
	{
		Dummy const * ptr = promote<Dummy>(reinterpret_cast<DUMMY const *>(obj));
		if(ptr == nullptr) {
			engine_log("obj_listvar received NULL object!");
			return;
		}
		engine_log("Properties(%p):", obj);
		for(auto const & prop : ptr->properties)
		{
			char buf[256];
			Property const & p = prop.second;
			switch(p.type)
			{
				case GL_FLOAT: std::sprintf(buf, "%f", p.data.floats[0]); break;
				case GL_FLOAT_VEC2: std::sprintf(buf, "(%f, %f)", p.data.floats[0], p.data.floats[1]); break;
				case GL_FLOAT_VEC3: std::sprintf(buf, "(%f, %f, %f)", p.data.floats[0],p.data.floats[1], p.data.floats[2]); break;
				case GL_FLOAT_VEC4: std::sprintf(buf, "(%f, %f, %f, %f)", p.data.floats[0],p.data.floats[1], p.data.floats[2], p.data.floats[3]); break;
				case GL_INT: std::sprintf(buf, "%d", p.data.ints[0]); break;
				case GL_INT_VEC2: std::sprintf(buf, "(%d, %d)", p.data.ints[0], p.data.ints[1]); break;
				case GL_INT_VEC3: std::sprintf(buf, "(%d, %d, %d)", p.data.ints[0],p.data.ints[1], p.data.ints[2]); break;
				case GL_INT_VEC4: std::sprintf(buf, "(%d, %d, %d, %d)", p.data.ints[0],p.data.ints[1], p.data.ints[2], p.data.ints[3]); break;
				default: strcpy(buf, "???"); break;
			}
			engine_log("\t%s = %s", prop.first.c_str(), buf);
		}
	}
}
