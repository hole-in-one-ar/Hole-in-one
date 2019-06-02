#version 440

in vec3 localPos, worldPos;

void main(void) {
	vec3 n = vec3(normalize(localPos.xy), 0);
	vec3 c = vec3(1,1,0);
	c *= smoothstep(1.2,0,-localPos.z) * 0.5 + 0.5;
	c *= smoothstep(0,0.1,-localPos.z) * 0.5 + 0.5;
	gl_FragColor = vec4(c,1);
}