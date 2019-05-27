#version 440

in vec3 position;

uniform float time;

out vec3 v;

void main(void) {
	vec3 p = position;
	float a = time * 0.01;
	p.xz *= mat2(cos(a),sin(a),-sin(a),cos(a));
	p.zy *= mat2(cos(a),sin(a),-sin(a),cos(a));
	p *= 0.5;
	gl_Position = vec4(p, 1.);
	v = position;
}