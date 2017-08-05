#include <stdio.h>
#include <acknext.h>

int main(int argc, char *argv[])
{
	if(engine_open(argc, argv) == false)
    {
		fprintf(stderr, "Failed to initialize engine: %s\n", engine_lasterror_text);
        return 1;
    }

	filesys_addResource("packed.zip", NULL);

	if(bmap_load("packed.png") != NULL) {
		engine_log("Load successful!");
	}

    while(engine_frame())
    {

    }

    engine_close();
    return 0;
}

