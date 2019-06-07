#version 440

in vec3 localPos, worldPos;
uniform sampler2D image;

vec3 sampleSky(vec3 n, float lod) {
	vec2 p = -n.xz / abs(n.y); // R^2
	float d = length(p);
	p = normalize(p) * (atan(d) / (3.1415926535/2.)); // D^2
	p.x /= 1.6 / 0.9;
	return pow(textureLod(image, p*0.5+0.5, lod).xyz, vec3(2.2));
}

void main(void) {
	vec3 n = vec3(normalize(localPos.xy), 0);
	vec3 c = vec3(1,1,0);
	// Diffuse
	c *= mix(sampleSky(n, 8), vec3(1.0), 0.2);
	// AO
	c *= 1.5 - pow(1 - 1 / (-localPos.z + 1.0), 5);
	// Corner
	c *= smoothstep(0,0.1,-localPos.z) * 0.5 + 0.5;
	gl_FragColor = vec4(c,1);
}