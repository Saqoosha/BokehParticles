#version 120

uniform sampler2DRect tex;
uniform float threshold = 0.5;
uniform float scale = 1.0;

void main() {
	gl_FragColor = max(texture2DRect(tex, gl_TexCoord[0].xy) - vec4(vec3(threshold), 0.0), vec4(0.0)) * vec4(scale);
}
