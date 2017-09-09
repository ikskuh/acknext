#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#include <engine.hpp>
#include <acknext/extension.h>

#include <list>
#include <string>

static const uint32_t ACKNEXT_MAGIC = 0xCC41E367;

class Extension
{
	friend bool ext_register(const char *name, EXTENSION *extension);

	 // this is kinda compiler specific, isn't it?
	template<typename T> friend class __gnu_cxx::new_allocator;
private:
	static std::list<Extension> extensions;
private:
	std::string name;
	EXTENSION * ext;
private:
	Extension(std::string const & name, EXTENSION * ext);
	~Extension() = default;
public:

	static void writeHeader(ACKFILE * file, ACKTYPE type, ACKGUID const & guid);

	static void * load(ACKFILE * file, ACKTYPE type);

	template<typename T>
	static T * load(ACKFILE * file);
};

#endif // EXTENSION_HPP
