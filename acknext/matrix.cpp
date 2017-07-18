#include "engine.h"

ACKFUN void mat_mul(MATRIX dst, MATRIX const lhs, MATRIX const rhs)
{
	MATRIX temp;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			var val = 0;
			for(int i = 0; i < 4; i++) {
				val += lhs[r][i] * rhs[i][c];
			}
			temp[r][c] = val;
		}
	}
	mat_copy(dst, temp);
}

ACKFUN void mat_id(MATRIX dst)
{
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			dst[r][c] = (r==c)?1:0;
		}
	}
}

ACKFUN void mat_copy(MATRIX dst, MATRIX const src)
{
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			dst[r][c] = src[r][c];
		}
	}
}

