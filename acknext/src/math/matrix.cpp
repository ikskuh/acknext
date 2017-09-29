#include <engine.hpp>

#include "../graphics/scene/ackglm.hpp"

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

	MATRIX * mat_invert(MATRIX * mat)
	{
		glm_to_ack(mat, glm::inverse(ack_to_glm(*mat)));
		return mat;
	}

	MATRIX * quat_to_mat(MATRIX * dst, QUATERNION const * src)
	{
		dst = mat_wrap(dst);
		glm_to_ack(dst, glm::mat4_cast(ack_to_glm(*src)));
		return dst;
	}


	MATRIX * mat_scale(MATRIX * mat, VECTOR const * scale)
	{
		if(!mat) mat = mat_id(NULL);
		glm_to_ack(mat, glm::scale(ack_to_glm(*mat), ack_to_glm(*scale)));
		return mat;
	}

	MATRIX * mat_translate(MATRIX * mat, VECTOR const * offset)
	{
		if(!mat) mat = mat_id(NULL);
		glm_to_ack(mat, glm::translate(ack_to_glm(*mat), ack_to_glm(*offset)));
		return mat;
	}

	MATRIX * mat_rotate(MATRIX * mat, QUATERNION const * rotation)
	{
		if(!mat) mat = mat_id(NULL);
		glm_to_ack(mat, ack_to_glm(*mat) * glm::mat4_cast(ack_to_glm(*rotation)));
		return mat;
	}
}
