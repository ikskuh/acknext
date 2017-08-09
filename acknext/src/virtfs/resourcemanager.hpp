#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <engine.hpp>

class ResourceManager
{
public:
	ResourceManager() = delete;

	static void initialize();

	static void shutdown();
};

#endif // RESOURCEMANAGER_HPP
