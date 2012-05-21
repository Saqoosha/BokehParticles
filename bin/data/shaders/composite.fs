#version 120

uniform sampler2DRect tex;

uniform vec2 aberration_center = vec2(512., 394.);
uniform float aberration_strength = 0.;
uniform vec2 vignette_center = vec2(512., 384.);
uniform float vignette_radius = 500.;
uniform float vignette_power = 1.;
uniform float exposure = 1.;
uniform float gamma_correction = 1.0 / 2.2;
uniform float time = 0;
uniform float noise_strength = .3;
uniform float segment_angle = 3.14159265358979323846264 / 3.;
uniform float source_angle_offset = 0.;
uniform float target_angle_offset = 0.;

#define NUM_BANDS 8
const vec4 bands[NUM_BANDS] = vec4[](
		vec4(0.049691828, 0.0,         0.181211048, 1.0 / NUM_BANDS),
		vec4(0.089102279, 0.012903528, 0.591176815, 1.0 / NUM_BANDS),
		vec4(0.0,         0.34202403,  0.227225912, 1.0 / NUM_BANDS),
		vec4(0.094029258, 0.486094799, 0.0,         1.0 / NUM_BANDS),
		vec4(0.421743901, 0.158977644, 0.0,         1.0 / NUM_BANDS),
		vec4(0.263851575, 0.0,         0.0,         1.0 / NUM_BANDS),
		vec4(0.067715218, 0.0,         0.0,         1.0 / NUM_BANDS),
		vec4(0.013865942, 0.0,         0.000386225, 1.0 / NUM_BANDS)
    );

vec2 kaleidoscope(vec2 original) {
   vec2 center = vec2(512., 384.);
   vec2 c = original - center;
   float radius = length(c);
   float angle = atan(c.y, c.x) + source_angle_offset;
   angle = mod(angle, segment_angle) + target_angle_offset;
   return vec2(cos(angle) * radius, sin(angle) * radius) + center;
}

vec4 color(vec4 ratio, float zoom) {
	vec2 c = (kaleidoscope(gl_TexCoord[0].xy) - aberration_center) / zoom + aberration_center;
	return texture2DRect(tex, c) * ratio;
}

vec4 chromaticAberration() {
	float s = aberration_strength / NUM_BANDS;
	float base = 1.0 + s * NUM_BANDS * .5;
	vec4 color = vec4(0.0);
	for (int i = 0; i < NUM_BANDS; i++) {
		color += color(bands[i], base - s * i);
	}
	return color;
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	vec4 color = chromaticAberration();

   // vignette
	vec2 p = (gl_TexCoord[0].xy - vignette_center) / (vignette_radius * 2.0);
	float vignette = 1.0 - dot(p, p);
	color *= vec4(vec3(clamp(pow(vignette, vignette_power), 0.0, 1.0)), 1.0);
	
   // exposure
	color.rgb = 1.0 - pow(vec3(2.71), -(color.rgb * exposure));

	gl_FragColor = vec4(pow(color.rgb * gl_Color.rgb, vec3(gamma_correction)), color.a * gl_Color.a);
	gl_FragColor.rgb *= 1. + (rand(gl_FragCoord.xy + time) - .5) * noise_strength;
}
