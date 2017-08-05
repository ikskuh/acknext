#include <stdio.h>
#include <acknext.h>

#include <GL/gl3w.h>

int main(int argc, char *argv[])
{
	if(engine_open(argc, argv) == false)
    {
		fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror_text);
        return 1;
    }

	MODEL * model = model_get("earth.mdl");

    while(engine_frame())
    {

    }

    engine_close();
    return 0;
}

