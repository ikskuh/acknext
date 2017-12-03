#ifndef ACKGLM_HPP
#define ACKGLM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <acknext.h>

static inline void glm_to_ack(MATRIX * target, const glm::mat4 & mat)
{
	for(int i = 0; i < 16; i++) {
		target->fields[i/4][i%4] = mat[i/4][i%4];
	}
}

static inline glm::mat4 ack_to_glm(MATRIX const & source)
{
	glm::mat4 mat;
	for(int i = 0; i < 16; i++) {
		mat[i/4][i%4] = source.fields[i/4][i%4];
	}
	return mat;
}


static inline void glm_to_ack(QUATERNION * target, const glm::quat & q)
{
	target->x = q.x;
	target->y = q.y;
	target->z = q.z;
	target->w = q.w;
}

static inline void glm_to_ack(VECTOR * target, const glm::vec3 & q)
{
	target->x = q.x;
	target->y = q.y;
	target->z = q.z;
}

static inline glm::vec3 ack_to_glm(const VECTOR & vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

static inline glm::quat ack_to_glm(const QUATERNION & quat) {
	return glm::quat(quat.w, glm::vec3(quat.x, quat.y, quat.z));
}

#endif // ACKGLM_HPP
