#ifndef ENGINECONFIG_H
#define ENGINECONFIG_H

#include <vector>
#include <string>
#include "acknext.h"

enum class FullscreenType
{
	Windowed,
	Fullscreen,
	DesktopFullscreen,
};

struct EngineConfig
{
	bool load(char const * fileName);

	EngineConfig();
	EngineConfig(EngineConfig const &) = delete;
	EngineConfig(EngineConfig &&) = delete;
	~EngineConfig() = default;

	std::vector<std::string> includePaths;
	std::vector<std::string> libraryPaths;
	std::vector<std::string> libraries;
	std::vector<std::string> sourceFiles;

	std::string title;
	FullscreenType fullscreen;
	SIZE resolution;
};

extern EngineConfig engine_config;

#endif // ENGINECONFIG_H
