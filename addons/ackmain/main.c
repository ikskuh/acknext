#include <stdio.h>
#include <assert.h>
#include <acknext.h>

extern void gamemain();

int main(int argc, char ** argv)
{
	assert(argc >= 1);
	engine_config.argv0 = argv[0];
	return engine_main(gamemain);
}
