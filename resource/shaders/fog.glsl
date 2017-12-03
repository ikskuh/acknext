// source: https://github.com/hughsk/glsl-fog
/**
 The MIT License (MIT)

 Copyright (c) 2013 Hugh Kennedy

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 **/

float fogFactorExp(const float dist, const float density) {
  return 1.0 - clamp(exp(-density * dist), 0.0, 1.0);
}

float fogFactorExp2(const float dist, const float density)
{
  const float LOG2 = -1.442695;
  float d = density * dist;
  return 1.0 - clamp(exp2(d * d * LOG2), 0.0, 1.0);
}

float fogFactorLinear(const float dist, const float start, const float end)
{
  return 1.0 - clamp((end - dist) / (end - start), 0.0, 1.0);
}

uniform vec4 vecFogColor; // rgb + density
uniform vec3 vecViewPos;

vec3 applyFog(vec3 position, vec3 surface)
{
	return mix(
		surface,
		vecFogColor.rgb,
		fogFactorExp2(length(position - vecViewPos), vecFogColor.a));
}
