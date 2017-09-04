#ifndef ENGINEOBJECT_HPP
#define ENGINEOBJECT_HPP

#include <GL/gl3w.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <map>
#include <string>

#include <acknext.h>

extern "C" void engine_log(char const * format, ...);

struct Property
{
	Property() = default;

	explicit Property(GLenum type, void const * data);

	void set(GLenum type, void const * data);

	GLenum type;
	union {
		int ints[4];
		float floats[4];
		BITMAP * texture;
	} data;
};

extern "C" void obj_listvar(void const*);

class BaseEngineObject
{
	friend void obj_listvar(void const *);
public:
	const uint32_t magic;
	mutable std::map<std::string, Property> properties;
protected:
	explicit BaseEngineObject();
	virtual ~BaseEngineObject();
public:
	void removeProperty(std::string const & name);

	void setProperty(std::string const & name, Property const & value);

	Property const & getProperty(std::string const & name) const;
};

template<typename T>
class EngineObject :
	public BaseEngineObject
{
public:
	typedef T cdataType;
	static const ptrdiff_t cdataOffset = sizeof(EngineObject<T>*);
private:
	T * cdata;
public:
	explicit EngineObject() : cdata(nullptr)
	{
		uintptr_t ptr = reinterpret_cast<uintptr_t>(malloc(cdataOffset + sizeof(T)));
		auto ** ref = reinterpret_cast<EngineObject<T>**>(ptr);
		*ref = this;
		this->cdata = reinterpret_cast<T*>(ptr + cdataOffset);
		// Initialize with explicit zeroes :)
		memset(this->cdata, 0, sizeof(T));
	}

	virtual ~EngineObject()
	{
		uintptr_t ptr = reinterpret_cast<uintptr_t>(this->cdata);
		ptr -= cdataOffset;
		free(reinterpret_cast<void*>(ptr));
	}

	T const & api() const { return *this->cdata; }
	T & api() { return *this->cdata; }
};

// A dummy structure
struct DUMMY { };
struct Dummy : public EngineObject<DUMMY> { };

template<typename T>
static inline T * promote(typename T::cdataType * value)
{
	if(value != nullptr) {
		uintptr_t ptr = reinterpret_cast<uintptr_t>(value);
		ptr	-= T::cdataOffset;
		EngineObject<typename T::cdataType> ** obj = reinterpret_cast<EngineObject<typename T::cdataType>**>(ptr);
		T * result = static_cast<T *>(*obj);
		if(result->magic != 0xBADC0DED) {
			engine_log("promote<T> received a non-engine object (%08X)!", result->magic);
			abort();
		}
		return result;
	} else {
		return nullptr;
	}
}

template<typename T>
static inline T const * promote(typename T::cdataType const * value)
{
	if(value != nullptr) {
		uintptr_t ptr = reinterpret_cast<uintptr_t>(value);
		ptr	-= T::cdataOffset;
		EngineObject<typename T::cdataType> * const * obj = reinterpret_cast<EngineObject<typename T::cdataType>*const *>(ptr);
		T const * result = static_cast<T const *>(*obj);
		if(result->magic != 0xBADC0DED) {
			engine_log("promote<T> received a non-engine object (%08X)!", result->magic);
			abort();
		}
		return result;
	} else {
		return nullptr;
	}
}

template<typename T>
static inline T * demote(EngineObject<T> * value)
{
	if(value != nullptr) {
		return &value->api();
	} else {
		return nullptr;
	}
}

#endif // ENGINEOBJECT_HPP
