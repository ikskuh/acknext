#include "engineconfig.h"

#include "dotconf.h"

EngineConfig engine_config;


EngineConfig::EngineConfig()
{
    resolution = { 1280, 720 };
    libraries = { };
    libraryPaths = { };
    includePaths = { };
    fullscreen = FullscreenType::Windowed;
    title = "Acknext Engine";
}

static DOTCONF_CB(op_title);
static DOTCONF_CB(op_fullscreen);
static DOTCONF_CB(op_resolution);
static DOTCONF_CB(op_libraryPath);
static DOTCONF_CB(op_includePath);
static DOTCONF_CB(op_libraries);

static const configoption_t options[] = {
	{"Window.Title",         ARG_STR,  op_title,       NULL, CTX_ALL},
    {"Window.Fullscreen",    ARG_STR,  op_fullscreen,  NULL, CTX_ALL},
    {"Window.Resolution",    ARG_LIST, op_resolution,  NULL, CTX_ALL},
    {"Compiler.LibraryPath", ARG_STR,  op_libraryPath, NULL, CTX_ALL},
    {"Compiler.IncludePath", ARG_STR,  op_includePath, NULL, CTX_ALL},
	{"Compiler.Libraries",   ARG_LIST, op_libraries,   NULL, CTX_ALL},
	LAST_CONTEXT_OPTION
};

int acknext_conf_errorhandler(configfile_t * configfile, int type, unsigned long dc_errno, const char *msg)
{
	switch(type)
	{
		default:
			engine_log("%s:%d: %s", configfile->filename, configfile->line, msg);
			break;
	}
	return ERR_NOERROR;
}

bool EngineConfig::load(char const * fileName)
{
	configfile_t *configfile = dotconf_create(
		(char*)fileName,
		options,
		NULL,
		CASE_INSENSITIVE | DONT_SUBSTITUTE);

	configfile->errorhandler = acknext_conf_errorhandler;
	if (!configfile)
	{
		engine_log("Failed to open config file %s!", fileName);
		return false;
	}

	bool success;
	if ((success = dotconf_command_loop(configfile)))
	{
		engine_log("Failed to parse configuration file!");
	}

	dotconf_cleanup(configfile);

	return success;
}

DOTCONF_CB(op_title)
{
	engine_config.title = std::string(cmd->data.str);
	return NULL;
}

DOTCONF_CB(op_fullscreen)
{
	return NULL;
}

DOTCONF_CB(op_resolution)
{
	return NULL;
}

DOTCONF_CB(op_libraryPath)
{
	engine_config.libraryPaths.emplace_back(cmd->data.str);
	return NULL;
}

DOTCONF_CB(op_includePath)
{
	engine_config.includePaths.emplace_back(cmd->data.str);
	return NULL;
}

DOTCONF_CB(op_libraries)
{
	for (int i = 0; i < cmd->arg_count; i++)
		engine_config.libraries.emplace_back(cmd->data.list[i]);
	return NULL;
}
