#include "engine.h"

#include <list>
#include <algorithm>
#include <string>

#include "libtcc.h"

struct SCRIPT
{
private:
    TCCState * const tcc;
public:
    SCRIPT() :
        tcc(tcc_new())
    {
        tcc_set_output_type(this->tcc, TCC_OUTPUT_MEMORY);
    }

    SCRIPT(SCRIPT const & other) = delete;
    SCRIPT(SCRIPT && other) = delete;

    ~SCRIPT()
    {
        tcc_delete(this->tcc);
    }
public:

    bool setup()
    {
        bool success = true;

		engine_log("Set up compiler...");
		for(auto & val : engine_config.at("includePath"))
		{
			engine_log("include path: %s", val.get<std::string>().c_str());
			success &= this->addIncludePath(val.get<std::string>().c_str());
		}
		for(auto & val : engine_config.at("libraryPath"))
		{
			engine_log("library path: %s", val.get<std::string>().c_str());
			success &= this->addLibraryPath(val.get<std::string>().c_str());
		}
		for(auto & val : engine_config.at("libraries"))
		{
			engine_log("library:      %s", val.get<std::string>().c_str());
			success &= this->addLibrary(val.get<std::string>().c_str());
		}
		engine_log("Compiler setup %s.", (success ? "successful" : "failed"));
        return success;
    }

#define TCC_CHECKED(code, message) if((code) == -1) { \
        engine_seterror( \
            COMPILATION_FAILED, \
            message); \
        return false; \
    } \
    return true

    bool addIncludePath(char const * path)
    {
        TCC_CHECKED(tcc_add_include_path(this->tcc, path), "Failed to add include path.");
    }

    bool addLibraryPath(char const * path)
    {
        TCC_CHECKED(tcc_add_library_path(this->tcc, path), "Failed to add library path.");
    }

    bool addLibrary(char const * lib)
    {
        TCC_CHECKED(tcc_add_library(this->tcc, lib), "Failed to add library.");
    }

    bool addFile(char const * file)
    {
        TCC_CHECKED(tcc_add_file(this->tcc, file), "Failed to compile file.");
    }

    bool compile(char const * source)
    {
        TCC_CHECKED(tcc_compile_string(this->tcc, source), "Failed to compile source.");
    }


    bool link()
    {
        TCC_CHECKED(tcc_relocate(this->tcc, TCC_RELOCATE_AUTO), "Failed to link script.");
    }

    void * getSymbol(char const * name) const
    {
        return tcc_get_symbol(this->tcc, name);
    }

    template<typename T>
    T getSymbol(char const * name) const
    {
        return reinterpret_cast<T>(tcc_get_symbol(this->tcc, name));
    }
};

ACKFUN SCRIPT * script_compile(char const * fileName)
{
    SCRIPT * tcc = new SCRIPT();

    if(tcc->setup() == false) {
        delete tcc;
        return nullptr;
    }

    if(tcc->addFile(fileName) == false) {
        delete tcc;
        return nullptr;
    }

    if(tcc->link() == false) {
        delete tcc;
        return nullptr;
    }

    // HACK: Well, this is quite hacky, but it should work :P
    return tcc;
}

ACKFUN SCRIPT * script_compile_src(char const * source)
{
    SCRIPT * tcc = new SCRIPT();

    if(tcc->setup() == false) {
        delete tcc;
        return nullptr;
    }

    if(tcc->compile(source) == false) {
        delete tcc;
        return nullptr;
    }

    if(tcc->link() == false) {
        delete tcc;
        return nullptr;
    }

    return tcc;
}

ACKFUN void * script_symbol(SCRIPT * script, char const * name)
{
    if(script == nullptr) {
        return nullptr;
    }
    return script->getSymbol(name);
}

static bool hasInitialFile = false;
static SCRIPT mainCompiler;

bool compiler_init()
{
    return mainCompiler.setup();
}

bool compiler_add(char const * fileName)
{
    hasInitialFile = true;
    return mainCompiler.addFile(fileName);
}

bool compiler_start()
{
    if(mainCompiler.link() == false) {
        return false;
    }

    if(hasInitialFile)
    {
        typedef void (*MainFunc)();

        MainFunc mainFunc = mainCompiler.getSymbol<MainFunc>("main");

        if(mainFunc == nullptr)
        {
            engine_log("Warning: The initial script does not provide a main() function!");
        }
        else
        {
            // We can savely ignore the handle here as we don't want to play with the main script at all ;)
            task_start(mainFunc, nullptr);
        }
    }
    return true;
}
