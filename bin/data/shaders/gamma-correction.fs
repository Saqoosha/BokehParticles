#version 120

uniform sampler2DRect tex;
uniform float gamma = 1.0 / 2.2;

void main() {
	vec4 color = texture2DRect(tex, gl_TexCoord[0].xy);
	gl_FragColor = vec4(pow(color.rgb, vec3(gamma)), color.a);
}
