#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <engine.hpp>

#include <vector>
#include <string>

enum class FullscreenType
{
	Windowed,
	Fullscreen,
	DesktopFullscreen,
};

struct Config
{
	bool load(char const * fileName);

	Config();
	NOCOPY(Config);
	~Config() = default;

	std::vector<std::string> includePaths;
	std::vector<std::string> libraryPaths;
	std::vector<std::string> libraries;
	std::vector<std::string> sourceFiles;

	std::string title;
	FullscreenType fullscreen;
	SIZE resolution;
};

extern Config engine_config;

#endif // CONFIG_HPP
