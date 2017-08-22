#include <acknext.h>

extern "C" void debug_tools();

void gamemain()
{
	view_create((RENDERCALL)render_scene_with_camera, camera);
	task_defer((ENTRYPOINT)debug_tools, NULL);
	event_attach(on_escape, (EVENTHANDLER)engine_shutdown);
	filesys_addResource("/home/felix/projects/acknext/project-z/resources/", "/");


	SHADER * shdTerrain = shader_create();
	shader_addFileSource(shdTerrain, VERTEXSHADER, "shaders/terrain.vert");
	shader_addFileSource(shdTerrain, FRAGMENTSHADER, "shaders/terrain.frag");
	shader_link(shdTerrain);
}

int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}
