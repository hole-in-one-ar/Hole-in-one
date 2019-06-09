#version 440

in vec3 position;

uniform vec3 cameraPos;
uniform vec3 o;
uniform float s;
uniform float a;
uniform mat4 V, P;

out vec3 localPos, worldPos;
out vec3 n;

void main(void) {
	vec3 p = vec3(0, position.xy * vec2(1,1));
	vec2 rot = normalize(cameraPos.xy - o.xy);	
	p.xy = vec2(rot.y, -rot.x) * p.y;
	p.xy *= mat2(cos(a), sin(a), -sin(a), cos(a));
	p = p * s + o;

	vec4 vertex = vec4(p, 1);
	vertex = P * V * vertex;
	gl_Position = vertex;

	localPos = position;
	worldPos = p;
	n = vec3(rot, 0);
}