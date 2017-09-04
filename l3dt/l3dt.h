#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct L3Heightfield
{
	int width, height;
	float * data; // The pixels are ordered in rows going west-to-east, with south-to-north row-order.
	float horizontalScale;
} L3Heightfield;

L3Heightfield * l3hf_decode(void const * memory, size_t length);

void l3hf_free(L3Heightfield * hf);

typedef union L3Attribute
{
	uint16_t value;
	struct {
		uint8_t landType;
		uint8_t climateID;
	} __attribute__((packed));
} __attribute__((packed)) L3Attribute;

typedef struct L3Attributefield
{
	int width, height;
	L3Attribute * data; // The pixels are ordered in rows going west-to-east, with south-to-north row-order.
} L3Attributefield;

L3Attributefield * l3af_decode(void const * memory, size_t length);

void l3af_free(L3Attributefield * af);
