#version 120

uniform sampler2DRect tex;

const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main() {
	gl_FragColor = texture2DRect(tex, gl_TexCoord[0].xy) * weight[0];
	for (int i = 1; i < 3; i++) {
		gl_FragColor += texture2DRect(tex, gl_TexCoord[0].xy + vec2(0.0, offset[i])) * weight[i];
		gl_FragColor += texture2DRect(tex, gl_TexCoord[0].xy - vec2(0.0, offset[i])) * weight[i];
	}
}
