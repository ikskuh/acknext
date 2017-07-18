#ifndef BITMAPDETAIL_H
#define BITMAPDETAIL_H

#include "engine.h"

struct BITMAPdetail
{
	BITMAP * const bitmap;
	GLuint id;
	GLuint format;

	BITMAPdetail(BITMAP * bitmap);
	BITMAPdetail(BITMAPdetail const &) = delete;
	BITMAPdetail(BITMAPdetail &&) = delete;
	~BITMAPdetail();
};

#endif // BITMAPDETAIL_H
