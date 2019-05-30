#version 440

in vec3 position;

uniform vec3 o;
uniform float s;
uniform mat4 V, P;

out vec3 localPos, worldPos;

void main(void) {
	vec3 p = position * s + o;

	vec4 vertex = vec4(p, 1);
	vertex = P * V * vertex;
	gl_Position = vertex;

	localPos = position;
	worldPos = p;
}