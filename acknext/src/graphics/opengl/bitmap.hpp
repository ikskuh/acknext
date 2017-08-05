#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <engine.hpp>

class Bitmap : public EngineObject<BITMAP>
{
public:
	GLuint id;
	GLenum target;
	GLenum type;
public:
	explicit Bitmap(TEXTURETYPE type);
	NOCOPY(Bitmap);
	~Bitmap();
};

#endif // BITMAP_HPP
