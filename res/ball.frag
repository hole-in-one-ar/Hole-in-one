#version 440

in vec3 localPos, worldPos;

void main(void) {
	vec3 c = normalize(localPos);
	gl_FragColor = vec4(c*0.5+0.5,1);
}