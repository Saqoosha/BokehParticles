uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;
uniform sampler2DRect tex3;
uniform sampler2DRect tex4;
uniform float strength;

void main() {
	vec4 bloom = texture2DRect(tex1, gl_TexCoord[0].xy * .5)
               + texture2DRect(tex2, gl_TexCoord[0].xy * .25)
               + texture2DRect(tex3, gl_TexCoord[0].xy * .125)
               + texture2DRect(tex4, gl_TexCoord[0].xy * .0625);
	gl_FragColor.rgb = texture2DRect(tex0, gl_TexCoord[0].xy).rgb + bloom.rgb * strength;
	gl_FragColor.a = 1.0;
	gl_FragColor *= gl_Color;
}
