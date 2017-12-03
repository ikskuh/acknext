#include <stdio.h>
#include <stdlib.h>

#include <acknext.h>
#include <acknext/librc.h>

int process_material(char const * infile, ACKFILE * outfile)
{
	rcMaterial * mtl = librc_load_mtl(infile);
	if(!mtl)
		return EXIT_FAILURE;

	librc_write_mtl(outfile, mtl);

	return EXIT_SUCCESS;
}
