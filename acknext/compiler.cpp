#include "engine.h"

#include <list>
#include <algorithm>

#include "libtcc.h"

class CompileUnit
{
private:
    TCCState * const tcc;
public:
    CompileUnit() :
        tcc(tcc_new())
    {
        tcc_set_output_type(this->tcc, TCC_OUTPUT_MEMORY);
    }

    CompileUnit(CompileUnit const & other) = delete;
    CompileUnit(CompileUnit && other) = delete;

    ~CompileUnit()
    {
        tcc_delete(this->tcc);
    }
public:

    bool setup()
    {
        bool success = true;
        success &= this->addIncludePath("/usr/include/");
        success &= this->addIncludePath("/home/felix/projects/tcc-0.9.26/include/");
        success &= this->addIncludePath("/home/felix/projects/acknext/include/");
        success &= this->addLibraryPath("/tmp/build-acknext-Desktop-Debug/acknext/");
        success &= this->addLibrary("acknext");
        success &= this->addLibrary("m");
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
    CompileUnit * tcc = new CompileUnit();

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
    return (SCRIPT*)tcc;
}

ACKFUN SCRIPT * script_compile_source(char const * source)
{
    CompileUnit * tcc = new CompileUnit();

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

    // HACK: Well, this is quite hacky, but it should work :P
    return (SCRIPT*)tcc;
}

ACKFUN void * script_symbol(SCRIPT * script, char const * name)
{
    CompileUnit * tcc = reinterpret_cast<CompileUnit*>(script);
    if(tcc == nullptr) {
        return nullptr;
    }
    return tcc->getSymbol(name);
}

static bool successfullInit = true;
static CompileUnit mainCompiler;

bool compiler_init()
{
    return mainCompiler.setup();
}

bool compiler_add(char const * fileName)
{
    return mainCompiler.addFile(fileName);
}

bool compiler_start()
{
    if(mainCompiler.link() == false) {
        return false;
    }

    typedef void (*MainFunc)();

    MainFunc mainFunc = mainCompiler.getSymbol<MainFunc>("main");

    if(mainFunc == nullptr)
    {
        engine_seterror(COMPILATION_FAILED, "The main script does not provide a main() function!");
        return false;
    }

    // We can savely ignore the handle here as we don't want to play with the main script at all ;)
    task_start(mainFunc, nullptr);

    return true;
}
