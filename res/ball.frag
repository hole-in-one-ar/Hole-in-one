#version 440

uniform vec3 cameraPos;
in vec3 localPos, worldPos;

void main(void) {
	vec3 n = normalize(localPos);
	vec3 viewDir = normalize(worldPos - cameraPos);
	vec3 c = vec3(1,0,0);
	// Diffuse
	c = c * (max(dot(n, vec3(0,0,1)),0) * 0.5 + 0.5);
	// Rim
	c += mix(0, 0.4, smoothstep(1,0,dot(viewDir,-n)));
	// AO
	c *= mix(1, 0.3, smoothstep(0.2,1,dot(n, vec3(0,0,-1))));
	c = sin(worldPos*20.0)*0.5+0.5;
	gl_FragColor = vec4(c,1);
}