#version 440

uniform vec3 cameraPos;
in vec3 localPos, worldPos;
uniform float i;

void main(void) {
	float d = length(localPos);
	if(d > 1. || d < i) discard;
	vec3 c = vec3(0,0.5,1);
	gl_FragColor = vec4(c,1);
}