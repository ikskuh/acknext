#include <stdio.h>
#include <acknext.h>

#include <GL/gl3w.h>

void gamemain()
{
	MODEL * model = model_get("earth.mdl");
}


int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

