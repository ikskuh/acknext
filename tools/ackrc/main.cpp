#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <libgen.h>
#include <unistd.h>
#include <acknext.h>

bool useAbsolutePaths = false;

int process_texture(char const * infile, ACKFILE * outfile);
int process_material(char const * infile, ACKFILE * outfile);

struct
{
	char const * name;
	int (*process)(char const*,ACKFILE*);
	char const * extension;
} targets[] = {
	{ "texture",  process_texture,  ".atx" },
	{ "material", process_material, ".amf" },
	{ NULL, NULL, NULL }
};

char * make_default(char const * infile, char const * extension)
{
	char * outfile = (char*)malloc(strlen(infile) + strlen(extension) + 1);
	strcpy(outfile, infile);

	if(!useAbsolutePaths) {
		outfile = basename(outfile);
	}

	char * end = strchr(outfile, '.');
	if(end) {
		strcpy(end, extension);
	} else {
		strcat(outfile, extension);
	}
	return outfile;
}

int main(int argc, char ** argv)
{
	engine_config.argv0 = argv[0];
	engine_config.flags &= ~USE_VFS;

	int opt;
	char * outfile = NULL;
	while ((opt = getopt(argc, argv, "Ro:")) != -1) {
		switch (opt) {
			case 'R':
				useAbsolutePaths = true;
				break;
			case 'o':
			   outfile = strdup(optarg);
			   break;
			default: /* '?' */
			   fprintf(stderr, "Usage: %s target [-o outfile] infile\n",
					   argv[0]);
			   exit(EXIT_FAILURE);
		}
	}

	if (optind == argc) {
		fprintf(stdout, "usage: %s target [-o output] infile\n", argv[0]);
		fprintf(stdout, "target may be one of: texture\n");
		exit(EXIT_FAILURE);
	}

	if ((optind + 1) >= argc) {
		fprintf(stderr, "Missing 'infile' parameter!\n");
		exit(EXIT_FAILURE);
	}

	char * target = argv[optind + 0];
	char * infile = argv[optind + 1];

	for(int i = 0; targets[i].name; i++)
	{
		if(strcmp(target, targets[i].name) == 0) {
			if(!outfile)
				outfile = make_default(infile, targets[i].extension);

			printf("%s → %s", targets[i].name, outfile);

			ACKFILE * fptr = file_open_write(outfile);
			if(fptr == nullptr) {
				fprintf(stderr, "Failed to create '%s'!", outfile);
				exit(EXIT_FAILURE);
			}

			int success = (targets[i].process)(infile, fptr);

			file_close(fptr);

			if(success != EXIT_SUCCESS)
				unlink(outfile);
			return success;
		}
	}

	fprintf(stderr, "Invalid resource target: %s", target);
	return EXIT_FAILURE;
}
