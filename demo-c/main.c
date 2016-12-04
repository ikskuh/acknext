#include <stdio.h>

#include <acknext.h>

#include <math.h>
#include <stdlib.h>

#include <libtcc.h>

void ackmain();
void turn_color();

int symbol = 42;

int main(int argc, char *argv[])
{
    // TCC Experiments:
    TCCState * tcc = tcc_new();

    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

    tcc_add_symbol(tcc, "answer", &symbol);

    tcc_add_include_path(tcc, "/usr/include/");
    tcc_add_include_path(tcc, "/home/felix/projects/tcc-0.9.26/include/");
    tcc_add_include_path(tcc, "/home/felix/projects/acknext/include/");

    tcc_add_library_path(tcc, "/tmp/build-acknext-Desktop-Debug/acknext/");
    tcc_add_library(tcc, "acknext");

    /*
    tcc_compile_string(tcc,
"#include <stdio.h>" "\n"
"#include <acknext.h>" "\n"
"extern int answer; "
"void func() { "
"   engine_log(\"Hello World!\");"
"   return answer; "
"}");
*/

    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            tcc_add_file(tcc, "main.c");
        }
    }

    int memsiz = tcc_relocate(tcc, NULL);
    printf("Scriptsize: %d\n", memsiz);

    void * memory = malloc(memsiz);
    tcc_relocate(tcc, memory);

    void (*func)() = tcc_get_symbol(tcc, "main");

    if(func == NULL)
    {
        fprintf(stderr, "'main' not found!\n") ;
    }

    tcc_delete(tcc);

    if(func == NULL)
    {
        free(memory);
        return 1;
    }

    // Engine Experiments
    if(engine_open(argc, argv) == false)
    {
        printf("Failed to initialize engine: %s\n", engine_lasterror(NULL));
        return 1;
    }
    start(ackmain, NULL);
    start(func, NULL);
    while(engine_frame())
    {
        // Now: Run!
    }
    engine_close();
    return 0;
}

void ackmain()
{
    float time = 0.0f;
    engine_log("red = %p", &screen_color.red);
    start(turn_color, &screen_color.red);
    while(true)
    {
        // screen_color.red = 128 + 127 * sin(time);
        screen_color.green = 128 + 127 * sin(time + 1.0);

        time += time_step;
        wait();
    }
}

void turn_color()
{
    float time = 0.0;
    while(true)
    {
        CONTEXT(uint8_t) = 128 + 127 * sin(time);
        time += time_step;
        wait();
    }
}
