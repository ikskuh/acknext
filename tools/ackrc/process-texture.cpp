#include <stdio.h>
#include <stdlib.h>

#include <acknext/librc.h>

int process_texture(char const * infile, ACKFILE * outfile)
{
	rcImage * img = librc_load_image(infile);
	if(!img)
		return EXIT_FAILURE;

	librc_write_image(outfile, img);

	librc_free_image(img);

	return EXIT_SUCCESS;
}
