#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <unistd.h>

struct header_t
{
	char id[8];
	uint32_t magic;
	uint32_t type;
	uint8_t guid[16];
} __attribute__((packed));

struct data_t
{
	uint32_t target;
	uint32_t format;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t pixelFormat;
	uint32_t pixelType;
	uint32_t bufferSize;
} __attribute__((packed));

static const uint8_t guid[] = {
    0xb4, 0x0f, 0x64, 0x26,
    0x2d, 0x6c, 0x44, 0xb3,
    0xbd, 0x3d, 0x62, 0x9f,
    0xaa, 0x5a, 0xef, 0xe7
};

static char const * vertexShader = "#version 330" "\n"
"layout(location = 0) in vec2 pos;" "\n"
"out vec2 uv;" "\n"
"void main()" "\n"
"{" "\n"
"	gl_Position = vec4(pos, 0, 1);" "\n"
"	uv.x = 0.5 + 0.5 * pos.x;" "\n"
"	uv.y = 0.5 + 0.5 * pos.y;" "\n"
"}";

static char const * fragmentShader = "#version 330" "\n"
"layout(location = 0) out vec4 color;" "\n"
"in vec2 uv;" "\n"
"uniform sampler2D tex;" "\n"
"void main()" "\n"
"{" "\n"
"	color = texture(tex, uv);" "\n"
// "	color.xyw = vec3(uv, 1);" "\n"
"}";


struct header_t header;
struct data_t data;
void * pixelData;

SDL_Window * window;
SDL_GLContext context;

GLuint vao, vbo, pgm, tex;

void load()
{
	glCreateTextures(data.target, 1, &tex);

	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(tex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	switch(data.target)
	{
		case GL_TEXTURE_1D:
			glTextureStorage1D(
				tex,
				1,
				data.format,
				data.width);
			glTextureSubImage1D(
				tex,
				0, 0, data.width,
				data.pixelFormat, data.pixelType,
				pixelData);
			break;
		case GL_TEXTURE_1D_ARRAY:
		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
			glTextureStorage2D(
				tex,
				1,
				data.format,
				data.width, data.height);
			glTextureSubImage2D(
				tex,
				0, 0, 0, data.width, data.height,
				data.pixelFormat, data.pixelType,
				pixelData);
			break;
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_3D:
			glTextureStorage3D(
				tex,
				1,
				data.format,
				data.width, data.height, data.depth);
			glTextureSubImage3D(
				tex,
				0, 0, 0, 0, data.width, data.height, data.depth,
				data.pixelFormat, data.pixelType,
				pixelData);
			break;
		default:
			abort();
	}
}

void init()
{
	glCreateBuffers(1, &vbo);
	glCreateVertexArrays(1, &vao);

	static const float quad[] =
	{
	    -1.0, -1.0,
	     1.0, -1.0,
	    -1.0,  1.0,
	     1.0,  1.0,
	};
	glNamedBufferData(vbo, sizeof(quad), quad, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao,
		0,
		2,
		GL_FLOAT,
		GL_FALSE,
		0);
	glVertexArrayAttribBinding(vao, 0, 1);

	glVertexArrayVertexBuffer(vao, 1, vbo, 0, 2 * sizeof(float));

	glBindVertexArray(vao);

	GLuint vsh, fsh;

	vsh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsh, 1, &vertexShader, NULL);
	glCompileShader(vsh);

	fsh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsh, 1, &fragmentShader, NULL);
	glCompileShader(fsh);

	static char buffer[2048];
	glGetShaderInfoLog(vsh, 2048, NULL, buffer);
	if(strlen(buffer)) fprintf(stderr, "vsh:%s\n", buffer);

	glGetShaderInfoLog(fsh, 2048, NULL, buffer);
	if(strlen(buffer)) fprintf(stderr, "fsh:%s\n", buffer);

	pgm = glCreateProgram();
	glAttachShader(pgm, vsh);
	glAttachShader(pgm, fsh);
	glLinkProgram(pgm);

	glUseProgram(pgm);

	glUniform1i(glGetUniformLocation(pgm, "tex"), 0);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	load();

	glBindTextureUnit(0, tex);
}

void present()
{
	glClearColor(1,0,1,1);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	SDL_GL_SwapWindow(window);
}

static void (APIENTRY error)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
{
	(void)source;
	(void)type;
	(void)id;
	(void)length;
	(void)userParam;
	if(severity == GL_DEBUG_SEVERITY_HIGH) {
		fprintf(stderr, "[OpenGL] %s", message);
	}
}


int main(int argc, char ** argv)
{

	int opt;
	bool fullscreen = false;
	while((opt = getopt(argc, argv, "f")) != -1)
	{
		switch(opt)
		{
			case 'f':
				fullscreen = true;
				break;
			default:
				fprintf(stderr, "Unknown option: %c\n", opt);
				exit(EXIT_FAILURE);
		}
	}
	if(optind >= argc)
	{
		fprintf(stderr, "Requires a single file!\n");
		exit(EXIT_FAILURE);
	}
	char const * srcfile = argv[optind];

	FILE * sourceFile = fopen(srcfile, "rb");
	if(!sourceFile)
	{
		fprintf(stderr, "File not found!\n");
		exit(EXIT_FAILURE);
	}

	assert(sourceFile != NULL);
	assert(fread(&header, 1, sizeof(header), sourceFile) == sizeof(header));

	// TODO: Validate header!

	assert(fread(&data, 1, sizeof(data), sourceFile) == sizeof(data));

	pixelData = malloc(data.bufferSize);
	assert(pixelData != NULL);
	assert(fread(pixelData, 1, data.bufferSize, sourceFile) == data.bufferSize);

	fclose(sourceFile);

	if(memcmp(header.id, "ACKNEXT", 8) != 0)
	{
		fprintf(stderr, "Invalid file header!\n");
		exit(EXIT_FAILURE);
	}

	if(header.magic != 0xCC41E367)
	{
		fprintf(stderr, "Invalid file header!\n");
		exit(EXIT_FAILURE);
	}

	if(header.type != 8) // TYPE_BITMAP
	{
		fprintf(stderr, "Invalid file type!\n");
		exit(EXIT_FAILURE);
	}

	if(memcmp(header.guid, guid, 16) != 0)
	{
		fprintf(stderr, "Unsupported file serialization (invalid GUID)!\n");
		exit(EXIT_FAILURE);
	}

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow(
		"Acknext Texture Viewer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		data.width, data.height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
	if(!window) {
		fprintf(stderr, "Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	context = SDL_GL_CreateContext(window);
	if(!context)
	{
		fprintf(stderr, "Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if(gl3wInit() < 0) {
		fprintf(stderr, "Failed: Invalid OpenGL version!\n");
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetSwapInterval(0);


	glDebugMessageCallback(&error, NULL);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	init();

	present();

	while(true)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					exit(EXIT_SUCCESS);
				case SDL_KEYDOWN:
					if(e.key.keysym.sym == SDLK_ESCAPE)
						exit(EXIT_SUCCESS);
					break;
			}
			present();
		}
		SDL_Delay(1);
	}

	return 0;
}
