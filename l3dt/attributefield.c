#include "l3dt.h"

#include <stdlib.h>
#include <string.h>

typedef struct AMFHeader
{
	uint8_t marker[4]; // Tells L3DT and humans that this is an L3DT file. Note string is not null terminated.
	uint16_t type;     // Tells L3DT what type of map file this is. Must be 520 for AMF.
	uint16_t width;    // East-west extent of the map (number of pixels).
	uint16_t height;   //	North-south extent of the map (number of pixels).
	// uint16_t * data // The pixels are ordered in rows going west-to-east, with south-to-north row-order.
} __attribute__((packed)) AMFHeader;

L3Attributefield * l3af_decode(void const * memory, size_t length)
{
	(void)length;
	AMFHeader const * header = memory;

	if(memcmp(header->marker, "L3DT", 4) != 0) {
		return NULL;
	}
	if(header->type != 520) {
		return NULL;
	}

	L3Attributefield * result = malloc(sizeof(L3Attributefield) + sizeof(uint16_t) * header->width * header->height);
	result->width = header->width;
	result->height = header->height;
	result->data = (uint16_t*)((uint8_t*)result + sizeof(L3Attributefield));

	memcpy(result->data, (uint8_t*)header + sizeof(AMFHeader), sizeof(uint16_t) * result->width * result->height);

	return result;
}

void l3af_free(L3Attributefield * af)
{
	free(af);
}
