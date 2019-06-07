#version 440

in vec3 position;

uniform vec3 o;
uniform float s;
uniform mat4 V, P;
uniform int seed;
uniform float time;
uniform vec3 cameraPos;

out vec3 localPos, worldPos;
out vec2 fragSeed;

float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) {
	float ix = round(position.x);
	vec2 local = position.xy - vec2(ix,0); // [-0.5,0.5]
	vec3 oo = o;
	oo.z -= 0.01;

	vec2 se = vec2(seed * 1.732, ix.x * 1.414);
	float force = rand(se+1.f) * 0.6 + 0.4;
	float angle = rand(se+2.f) * 3.1415926535 * 2.0;
	float pitch = rand(se+3.f) * 0.3 + 0.1;
	vec3 velocity = vec3(
		cos(angle) * sin(pitch),
		sin(angle) * sin(pitch),
		cos(pitch)
	) * force * 3.0;
	float gravity = 0.01f;
	float t = (1 - pow(max(0, 1 - time/5), 4)) * 5;
	float ot = 0.01;
	oo += velocity * (t+ot) * s - gravity * (t+ot) * (t+ot) / 2 * vec3(0,0,1);
	float dur = clamp(3 + rand(se+4.f) * 2. - t, 0, 1);
	float size = (1. - pow(1. - dur, 4.0)) * rand(se+5.f);

	vec3 p = vec3(0, local.xy*size);
	vec2 rot = normalize(cameraPos.xy - oo.xy);
	p.xy = vec2(rot.y, -rot.x) * p.y;

	p = p * s + oo;

	vec4 vertex = vec4(p, 1);
	vertex = P * V * vertex;
	gl_Position = vertex;

	localPos = vec3(local, 0);
	worldPos = p;
	fragSeed = se;
}