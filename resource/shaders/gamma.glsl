#version 330

uniform float fGamma;

vec3 toLinear(vec3 v) {
	return pow(v, vec3(fGamma));
}

vec4 toLinear(vec4 v) {
	return pow(v, vec4(fGamma));
}



vec3 toGamma(vec3 v) {
	return pow(v, vec3(1.0 / fGamma));
}

vec4 toGamma(vec4 v) {
	return pow(v, vec4(1.0 / fGamma));
}
