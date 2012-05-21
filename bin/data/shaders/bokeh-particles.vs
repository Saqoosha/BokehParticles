#version 120

attribute float rotation;

uniform float shininess = 128.;
uniform float specular_strength = 100.;
uniform float diffuse = 1.;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	gl_Position = ftransform();
	vec4 blink = vec4(vec3(1. + pow(rand(gl_Position.xy), 3.) * 3.), 1.);
   vec3 light_vector = vec3(1., 0., 0.);
   vec3 position = vec3(gl_ModelViewMatrix * gl_Vertex);
   vec3 view_vector = normalize(-position);
   vec3 face_nomal = vec3(cos(rotation), 0., sin(rotation));
   vec3 reflection_vector = reflect(-light_vector, face_nomal);
   float specular = pow(abs(dot(view_vector, reflection_vector)), shininess)* specular_strength;
	gl_FrontColor = (gl_Color * blink) * vec4(vec3(diffuse + specular), 1.);
}
