#include <engine.hpp>

#include <functional>

static COLOR * temp(COLOR * in = nullptr)
{
	if(in) return in;
	static COLOR buffer[ACKNEXT_TEMP_COLORS];
	static int index = 0;
	return &buffer[index++ % ACKNEXT_TEMP_COLORS];
}

static void apply(COLOR * c, std::function<var(var)> const & a)
{
	c->red = a(c->red);
	c->green = a(c->green);
	c->blue = a(c->blue);
	c->alpha = a(c->alpha);
}

ACKNEXT_API_BLOCK
{
	COLOR * color_rgba(int r, int g, int b, int a)
	{
		COLOR * c = temp();
		c->red = var(r & 0xFF) / var(0xFF);
		c->green = var(g & 0xFF) / var(0xFF);
		c->blue = var(b & 0xFF) / var(0xFF);
		c->alpha = var(a & 0xFF) / var(0xFF);
		return c;
	}

	COLOR * color_rgb(int r, int g, int b)
	{
		return color_rgba(r,g,b,255);
	}

	COLOR * color_hex(int hexcode)
	{
		return color_rgb(hexcode >> 16, hexcode >> 8, hexcode);
	}

	COLOR * color_hexa(int hexcode)
	{
		return color_rgba(hexcode >> 16, hexcode >> 8, hexcode, hexcode >> 24);
	}

	COLOR * color_addgamma(COLOR * c)
	{
		apply(c, [](var v) { return pow(v, screen_gamma); });
		return c;
	}

	COLOR * color_removegamma(COLOR * c)
	{
		apply(c, [](var v) { return pow(v, 1.0 / screen_gamma); });
		return c;
	}

	COLOR * color_blend(COLOR * dst, COLOR const * left, COLOR const * right, var f)
	{
		dst = temp(dst);

		COLOR l = *left;
		COLOR r = *right;
		color_removegamma(&l);
		color_removegamma(&r);

		dst->red = lerp(l.red, l.red, f);
		dst->green = lerp(l.green, l.green, f);
		dst->blue = lerp(l.blue, l.blue, f);
		dst->alpha = lerp(l.alpha, l.alpha, f);

		return color_addgamma(dst);
	}
}
