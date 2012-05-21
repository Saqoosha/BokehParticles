#version 120

uniform vec2 resolution = vec2(1024., 768.);
uniform float focal_length = 50;
uniform float aparture = 50 / 2.8;
uniform float focus_plane = 2000;
uniform float mm_per_pixel;

const float kMaxRadius = 256.;

void main() {
	float coc = aparture * (focal_length * (focus_plane - gl_PositionIn[0].z + 1e-5)) / (gl_PositionIn[0].z * (focus_plane - focal_length));
	float radius = min(abs(coc) / mm_per_pixel + 1.5, kMaxRadius) * sign(coc);
   vec2 size = radius / resolution * gl_PositionIn[0].w;
   vec4 color = vec4(gl_FrontColorIn[0].rgb * 100. / (radius * radius) * min(1., gl_FrontColorIn[0].a), 1.0);

	gl_Position = gl_PositionIn[0] + vec4(-size.x, -size.y, 0.0, 0.0);
	gl_FrontColor = color;
	gl_TexCoord[0] = vec4(0.0, 1.0, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_PositionIn[0] + vec4(-size.x, size.y, 0.0, 0.0);
	gl_FrontColor = color;
	gl_TexCoord[0] = vec4(0.0, 0.0, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_PositionIn[0] + vec4(size.x, -size.y, 0.0, 0.0);
	gl_FrontColor = color;
	gl_TexCoord[0] = vec4(1.0, 1.0, 0.0, 0.0);
	EmitVertex();
	gl_Position = gl_PositionIn[0] + vec4(size.x, size.y, 0.0, 0.0);
	gl_FrontColor = color;
	gl_TexCoord[0] = vec4(1.0, 0.0, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}
