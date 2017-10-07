#include "librc.h"
#include <stb_image.h>

C_API void librc_write_image(ACKFILE * outfile, rcImage const * image)
{
	assert(outfile);
	assert(image);

	file_write_header(outfile, TYPE_BITMAP, acff_guidBitmap);

	file_write_uint32(outfile, GL_TEXTURE_2D);
	file_write_uint32(outfile, image->textureFormat);

	file_write_uint32(outfile, image->width);
	file_write_uint32(outfile, image->height);
	file_write_uint32(outfile, image->depth);

	file_write_uint32(outfile, image->pixelFormat);
	file_write_uint32(outfile, image->pixelType);
	file_write_uint32(outfile, image->bufsiz);

	file_write(outfile, image->pixels, image->bufsiz);
}

C_API rcImage * librc_read_image(ACKFILE * src)
{
	assert(src);

	return nullptr;
}

C_API rcImage * librc_load_image(char const * fileName)
{
	assert(fileName);

	FILE * file = fopen(fileName, "rb");
	if(!file) {
		LOG("Failed to open %s", fileName);
		return nullptr;
	}

	int w,h,n;

	float * hdr = nullptr;
	unsigned char * ldr = nullptr;

	if(stbi_is_hdr_from_file(file))
	{
		fseek(file, 0, SEEK_SET);
		hdr = stbi_loadf_from_file(file, &w, &h, &n, 0);
		if(!hdr) {
			LOG("Failed to load %s as a HDR image", fileName);
			fclose(file);
			return nullptr;
		}
	}
	else
	{
		fseek(file, 0, SEEK_SET);
		ldr = stbi_load_from_file(file, &w, &h, &n, 0);
		if(!ldr) {
			LOG("Failed to load %s as an image", fileName);
			fclose(file);
			return nullptr;
		}
	}

	assert(hdr || ldr);

	rcImage * img = (rcImage*)malloc(sizeof(rcImage));
	assert(img);

	img->target = GL_TEXTURE_2D;
	img->width = w;
	img->height = h;
	img->depth = 1;

	switch(n)
	{
		case 1:
			img->textureFormat = hdr ? GL_R32F : GL_R8;
			img->pixelFormat = GL_RED;
			break;
		case 3:
			img->textureFormat = hdr ? GL_RGB32F : GL_RGB8;
			img->pixelFormat = GL_RGB;
			break;
		case 4:
			img->textureFormat = hdr ? GL_RGBA32F : GL_RGBA8;
			img->pixelFormat = GL_RGBA;
			break;
		default:
			abort();
	}
	img->pixelType = hdr ? GL_FLOAT : GL_UNSIGNED_BYTE;
	img->pixels = hdr ? (void*)hdr : (void*)ldr;
	img->bufsiz = (hdr ? 4 : 1) * n * w * h;

	// Flip vertically
	{
		const size_t stride = n * w * (hdr ? 4 : 1);
		uint8_t scanline[stride];
		uint8_t * pixels = (uint8_t*)img->pixels;
		for(uint y = 0; y < h/2; y++)
		{
			void * lower = &pixels[y * stride];
			void * upper = &pixels[(h - y - 1) * stride];
			memcpy(scanline, lower,    stride);
			memcpy(lower,    upper,    stride);
			memcpy(upper,    scanline, stride);
		}
	}

	return img;
}

C_API void librc_free_image(rcImage * image)
{
	free(image->pixels);
	free(image);
}
