#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <libgen.h>

bool useAbsolutePaths = false;

int process_texture(char const * infile, char const * outfile);

char * make_default(char const * infile, char const * extension)
{
	char * outfile = malloc(strlen(infile) + strlen(extension) + 1);
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

struct
{
	char const * name;
	int (*process)(char const*,char const *);
	char const * extension;
} targets[] = {
	{ "texture", process_texture, ".atx" },
	{ NULL, NULL, NULL }
};

int main(int argc, char ** argv)
{
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
			return (targets[i].process)(infile, outfile);
		}
	}

	fprintf(stderr, "Invalid resource target: %s", target);
	return EXIT_FAILURE;
}
