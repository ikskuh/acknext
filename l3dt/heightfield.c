#include "l3dt.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

int min(int a, int b) {
	return (a<b)?a:b;
}

typedef struct Hf2Header
{
// 	File ID	0	4	string	Should be “HF2” (null terminated)
	char magic[4];
//	Version no.	4	2	unsigned short	Should be 0.
	uint16_t version;
//	Width	6	4	unsigned long	Width of map in pixels.
	uint32_t width;
//	Height	10	4	unsigned long	Height of map in pixels.
	uint32_t height;
//	Tile size	14	2	unsigned short	Size of internal map tiles (8→65535). Default is 256.
	uint16_t tileSize;
//	Vert. precis.	16	4	float	Precision of vertical scale, in metres. Must be greater than zero. Default is 0.01.
	float verticalPrecision;
//	Horiz. scale	20	4	float	Horizontal pixel spacing, in metres. Must be greater than zero. Default is 1.
	float horizontalScale;
//	Ext. header length	24	4	unsigned long	Length of extended header, in bytes. Zero is default.
	uint32_t extHeaderLen;
} __attribute__((packed)) Hf2Header;

typedef struct Hf2ExtHeader
{
	char type[4];    // Null-terminated string describing type of block. Typically “txt”, “xml”, or “bin”. Not case-sensitive.
	char name[16];   // Null-terminated string containing name of block. If null, the block should be read and stored but not interpreted.
	uint32_t length; // Number of bytes in the block, immediately following block length. May be zero.
	// virtual void* // User defined data. Length is given by 'Block length'. May be text, XML, binary, whatever.
} __attribute__((packed)) Hf2ExtHeader;

typedef struct Hf2TileHeader
{
	float verticalScale;  // The vertical scaling of the data in the tile.
	float verticalOffset; // The vertical offset of the data in the tile.
} __attribute__((packed)) Hf2TileHeader;

typedef struct Hf2LineHeader
{
	uint8_t depth; // The byte-depth used for difference encoding in the line. May be 1, 2 or 4.
	int32_t first; // The starting value of the line, encoded as described below.
} __attribute__((packed)) Hf2LineHeader;


L3Heightfield * l3hf_decode(void const * memory, size_t length)
{
	(void)length;
	Hf2Header const * header = memory;

	// engine_log("HF2 Header:");
	// engine_log("\tID             \t%s", header->magic);
	// engine_log("\tVersion        \t%d", header->version);
	// engine_log("\tWidth          \t%d", header->width);
	// engine_log("\tHeight         \t%d", header->height);
	// engine_log("\tTile Size      \t%d", header->tileSize);
	// engine_log("\tVert. Precision\t%f", header->verticalPrecision);
	// engine_log("\tHoriz. Scale   \t%f", header->horizontalScale);
	// engine_log("\tExt. Header Len\t%d", header->extHeaderLen);

	uint32_t numBytes = header->extHeaderLen;
	char const * ptr = memory;
	ptr += sizeof(Hf2Header);
	while(numBytes > 0)
	{
		Hf2ExtHeader const * ehdr = (void*)ptr;
		ptr += sizeof(Hf2ExtHeader);
		void const * data = ptr;
		ptr += ehdr->length;

		(void)data;

		// engine_log("Extended Header:");
		// engine_log("\tType: %s", ehdr->type);
		// engine_log("\tName: %s", ehdr->name);
		// engine_log("\tSize: %lu", ehdr->length);

		numBytes -= sizeof(Hf2ExtHeader);
		numBytes -= ehdr->length;
	}

	if((header->width & (header->width - 1)) || (header->height & (header->height - 1)))
	{
		// engine_log("Map size must be power of two!");
		// engine_shutdown();
		return NULL;
	}

	L3Heightfield * result = malloc(sizeof(L3Heightfield) + sizeof(float) * header->width * header->height);

	result->width = header->width;
	result->height = header->height;
	result->horizontalScale = header->horizontalScale;
	result->data = (float*)((uint8_t*)result + sizeof(L3Heightfield));

	int tileCountW = (header->width + header->tileSize - 1) / header->tileSize;
	int tileCountH = (header->height + header->tileSize - 1) / header->tileSize;

	for(int ty = 0; ty < tileCountH; ty++) // south-to-north
	{
		for(int tx = 0; tx < tileCountW; tx++) // west-to-east
		{
			Hf2TileHeader * thdr = (void*)ptr;
			ptr += sizeof(Hf2TileHeader);

			int countX = min(header->tileSize * (tx + 1), header->width) - header->tileSize * tx;
			int countY = min(header->tileSize * (ty + 1), header->width) - header->tileSize * ty;

			// engine_log("read tile (%d,%d) with size (%d, %d)", tx, ty, countX, countY);

			for(int l = 0; l < countY; l++) // south-to-north
			{
				Hf2LineHeader * lhdr = (void*)ptr;
				ptr += sizeof(Hf2LineHeader);

				int32_t lastValue = 0;
				for(int i = 0; i < countX; i++) // west-to-east
				{
					uint32_t fileValue = 0;
					if(i == 0) {
						fileValue = lhdr->first;
					} else {
						switch(lhdr->depth) {
							case 1:
								fileValue = *((int8_t*)ptr);
								break;
							case 2:
								fileValue = *((int16_t*)ptr);
								break;
							case 4:
								fileValue = *((int32_t*)ptr);
								break;
							default: abort();
						}
						ptr += lhdr->depth;
					}

					int32_t currentValue = fileValue + lastValue;

					float h = (float)currentValue * thdr->verticalScale + thdr->verticalOffset;
					uint x = header->tileSize * tx + i;
					uint y = header->tileSize * ty + l;
					if(x >= header->width || y >= header->height) abort();

					result->data[y * header->width + x] = h;

					lastValue = currentValue;
				}
			}
		}
	}

	return result;
}


static float lerp(float a, float b, float f)
{
	return a*(1-f) + b*f;
}

#define DATA(hf,x,y) hf->data[(y)*hf->width+(x)]

float l3hf_get(L3Heightfield const * hf, float x, float y)
{
	if(hf == NULL) {
		return 0.0;
	}
	x /= hf->horizontalScale;
	y /= hf->horizontalScale;
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x > (hf->width - 1)) x = (hf->width - 1);
	if(y > (hf->height - 1)) y = (hf->height - 1);

	int x0 = floor(x);
	int x1 = x0 + 1;
	int y0 = floor(y);
	int y1 = y0 + 1;

	return DATA(hf, x0, y0);

	float dx = x - x0;
	float dy = y - y0;

	float h00 = DATA(hf, x0, y0);
	float h01 = DATA(hf, x0, y1);
	float h10 = DATA(hf, x1, y0);
	float h11 = DATA(hf, x1, y1);

	return lerp(
		lerp(h00, h01, dy),
		lerp(h10, h11, dy),
		dx);
}

void l3hf_free(L3Heightfield * hf)
{
	free(hf);
}
