#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <engine.hpp>

class Bitmap : public EngineObject<BITMAP>
{
public:
	explicit Bitmap(GLenum type, GLenum format);
	NOCOPY(Bitmap);
	~Bitmap();
};

#endif // BITMAP_HPP
