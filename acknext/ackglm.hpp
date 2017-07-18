#ifndef ACKGLM_HPP
#define ACKGLM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

static inline void glm_to_ack(MATRIX target, const glm::mat4 & mat)
{
	for(int i = 0; i < 16; i++) {
		target[i/4][i%4] = mat[i/4][i%4];
	}
}

static inline glm::vec3 ack_to_glm(const VECTOR & vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

static inline glm::quat ack_to_glm(const QUATERNION & quat) {
	return glm::quat(quat.x, quat.y, quat.z, quat.w);
}

#endif // ACKGLM_HPP
