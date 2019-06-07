#version 440

in vec3 position;

uniform vec3 o;
uniform vec3 n;
uniform float s;
uniform mat4 V, P;

out vec3 localPos, worldPos;
out vec3 normal;
out float ao;

void main(void) {
	vec3 p = position;
	vec3 N = n;
	vec3 U = vec3(0,0,1);
	if(dot(N,U) > 0.99) U = vec3(1,0,0);
	vec3 T = normalize(cross(N, U));
	vec3 B = cross(N, T);
	p = mat3(T,B,N) * p;
	p = p * s + o;

	vec4 vertex = vec4(p, 1);
	vertex = P * V * vertex;
	gl_Position = vertex;

	localPos = position;
	worldPos = p;

	if(length(position.xy) < 1.01) {
		normal = vec3(0,0,1);
		ao = 1.0 - length(position.xy);
	} else {
		normal = normalize(vec3(normalize(position.xy),1));
		ao = distance(position.z, -1.0);
	}
	normal = mat3(T,B,N) * normal;
}