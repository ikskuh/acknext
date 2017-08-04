#include "engine.h"

ACKFUN void mat_mul(MATRIX * dst, MATRIX const * lhs, MATRIX const * rhs)
{
	MATRIX temp;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			var val = 0;
			for(int i = 0; i < 4; i++) {
				val += lhs->v[r][i] * rhs->v[i][c];
			}
			temp.v[r][c] = val;
		}
	}
	mat_copy(dst, &temp);
}

ACKFUN void mat_id(MATRIX * dst)
{
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			dst->v[r][c] = (r==c)?1:0;
		}
	}
}

ACKFUN void mat_copy(MATRIX * dst, MATRIX const * src)
{
	memcpy(dst, src, sizeof(MATRIX));
}

