#include <engine.hpp>

static MATRIX * mat_wrap(MATRIX * src)
{
	static int index;
	static MATRIX buffer[ACKNEXT_TEMP_MATRICES];
	if(src) {
		return src;
	} else {
		return &buffer[index++ % ACKNEXT_TEMP_MATRICES];
	}
}

ACKNEXT_API_BLOCK
{
	MATRIX * mat_mul(MATRIX * dst, MATRIX const * lhs, MATRIX const * rhs)
	{
		dst = mat_wrap(dst);
		MATRIX temp;
		for(int r = 0; r < 4; r++) {
			for(int c = 0; c < 4; c++) {
				var val = 0;
				for(int i = 0; i < 4; i++) {
					val += lhs->fields[r][i] * rhs->fields[i][c];
				}
				temp.fields[r][c] = val;
			}
		}
		mat_copy(dst, &temp);
		return dst;
	}

	MATRIX * mat_id(MATRIX * dst)
	{
		dst = mat_wrap(dst);
		for(int r = 0; r < 4; r++) {
			for(int c = 0; c < 4; c++) {
				dst->fields[r][c] = (r==c)?1:0;
			}
		}
		return dst;
	}

	MATRIX * mat_copy(MATRIX * dst, MATRIX const * src)
	{
		dst = mat_wrap(dst);
		memcpy(dst, src, sizeof(MATRIX));
		return dst;
	}
}
