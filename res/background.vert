#version 440

in vec3 position;

uniform vec3 o;
uniform float s;
uniform mat4 V, P;

out vec2 uv;

void main(void) {
	uv = position.xy * 0.5 + 0.5;
	gl_Position = vec4(position, 1);
}