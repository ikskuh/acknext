#include "engineobject.hpp"
#include "engine.hpp"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

Property::Property(GLenum type, Value const & value)
{
	this->set(type, value);
}

void Property::set(GLenum type, Value const & value)
{
	this->type = type;
	this->data = value;
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
	void obj_setvar(void * obj, char const * name, GLenum type, ...)
	{
		va_list list;
		va_start(list, type);

		Value value;
		if(Property::isSampler(type))
		{
			value.texture = va_arg(list, BITMAP*);
		}
		else
		{
			switch(type)
			{
				case ACK_POINTER:
					value.pointer = va_arg(list, void*);
					break;
#define MAKEVECS(_Name, _Type, _Var) \
				case GL_##_Name: \
					value._Var[0] = va_arg(list, _Type); \
					break; \
				case GL_##_Name##_VEC2: \
					value._Var[0] = va_arg(list, _Type); \
					value._Var[1] = va_arg(list, _Type); \
					break; \
				case GL_##_Name##_VEC3: \
					value._Var[0] = va_arg(list, _Type); \
					value._Var[1] = va_arg(list, _Type); \
					value._Var[2] = va_arg(list, _Type); \
					break; \
				case GL_##_Name##_VEC4: \
					value._Var[0] = va_arg(list, _Type); \
					value._Var[1] = va_arg(list, _Type); \
					value._Var[2] = va_arg(list, _Type); \
					value._Var[3] = va_arg(list, _Type); \
					break
				MAKEVECS(BOOL, int, bools);
				MAKEVECS(INT, int, ints);
				MAKEVECS(UNSIGNED_INT, uint, uints);
				MAKEVECS(FLOAT, double, floats);
				MAKEVECS(DOUBLE, double, floats);
#undef MAKEVECS
				case GL_FLOAT_MAT4:
					value.matrices[0] = va_arg(list, MATRIX);
					break;
				default:
					engine_log("0x%04X is an unsupported property type!", type);
					abort();
			}
		}
		va_end(list);

		Dummy * ptr = promote<Dummy>(reinterpret_cast<DUMMY*>(obj));
		ARG_NOTNULL(obj,);
		ptr->setProperty(name, Property(type, value));
	}

	void const * obj_getvar(void * obj, char const * name, GLenum * type)
	{
		Dummy * ptr = promote<Dummy>(reinterpret_cast<DUMMY*>(obj));
		ARG_NOTNULL(obj, nullptr);
		ARG_NOTNULL(name, nullptr);
		auto const & prop = ptr->getProperty(name);
		if(type != nullptr) {
			*type = prop.type;
		}
		if(prop.type == GL_NONE) {
			engine_seterror(ERR_INVALIDOPERATION, "The property %s does not exist!", name);
			return nullptr;
		}
		return &prop.data;
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
			char const * name = prop.first.c_str();
			Property const & p = prop.second;
			if(p.isSampler())
			{
				if(p.data.texture) {
					engine_log("\t%s = Texture(%d)", name, p.data.texture->object);
				} else {
					engine_log("\t%s = <null>", name);
				}
			}
			else
			{
				switch(p.type)
				{
	#define PRINTVEC(_Name, _Filter, _Var) \
					case GL_##_Name: \
						engine_log("\t%s = " _Filter, name, p.data._Var[0]); \
						break; \
					case GL_##_Name##_VEC2: \
						engine_log("\t%s = (" _Filter ", " _Filter ")", name, p.data._Var[0], p.data._Var[1]); \
						break; \
					case GL_##_Name##_VEC3: \
						engine_log("\t%s = (" _Filter ", " _Filter ", " _Filter ")", name, p.data._Var[0], p.data._Var[1], p.data._Var[2]); \
						break; \
					case GL_##_Name##_VEC4: \
						engine_log("\t%s = (" _Filter ", " _Filter ", " _Filter ", " _Filter ")", name, p.data._Var[0], p.data._Var[1], p.data._Var[2], p.data._Var[3]); \
						break

					PRINTVEC(BOOL, "%d", bools);
					PRINTVEC(INT, "%d", ints);
					PRINTVEC(UNSIGNED_INT, "%u", uints);
					PRINTVEC(FLOAT, "%f", floats);
					PRINTVEC(DOUBLE, "%f", floats);
	#undef PRINTVEC
					default:
						engine_log("\t%s = <Display not supported>", name);
						break;
				}
			}
		}
	}
}

bool Property::isSampler(GLenum value)
{
	switch(value)
	{
		case GL_SAMPLER_1D:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_CUBE_MAP_ARRAY:
		case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
			return true;
		case GL_INT_SAMPLER_1D:
		case GL_INT_SAMPLER_1D_ARRAY:
		case GL_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_INT_SAMPLER_2D_RECT:
		case GL_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_BUFFER:
		case GL_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
			return true;
		case GL_UNSIGNED_INT_SAMPLER_1D:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
			return true;
		default:
			return false;
	}
}
