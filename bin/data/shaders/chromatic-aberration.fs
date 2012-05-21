#version 120

uniform sampler2DRect src_tex_unit0;
uniform vec2 center;
uniform float strength;

#define NUM_BANDS 8
const vec4 bands[NUM_BANDS] = vec4[] (
		vec4(0.049691828, 0.0,         0.181211048, 1.0 / NUM_BANDS),
		vec4(0.089102279, 0.012903528, 0.591176815, 1.0 / NUM_BANDS),
		vec4(0.0,         0.34202403,  0.227225912, 1.0 / NUM_BANDS),
		vec4(0.094029258, 0.486094799, 0.0,         1.0 / NUM_BANDS),
		vec4(0.421743901, 0.158977644, 0.0,         1.0 / NUM_BANDS),
		vec4(0.263851575, 0.0,         0.0,         1.0 / NUM_BANDS),
		vec4(0.067715218, 0.0,         0.0,         1.0 / NUM_BANDS),
		vec4(0.013865942, 0.0,         0.000386225, 1.0 / NUM_BANDS)
    );

vec4 color(vec4 ratio, float zoom) {
	vec2 c = (gl_TexCoord[0].xy - center) / zoom + center;
	return texture2DRect(src_tex_unit0, c) * ratio;
}

void main() {
	float s = strength / NUM_BANDS;
	vec4 color = vec4(0.0);
	for (int i = 0; i < NUM_BANDS; i++) {
		color += color(bands[i], 1. + s * i);
	}
	gl_FragColor = color;
}
