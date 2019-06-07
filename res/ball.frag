#version 440

uniform vec3 cameraPos;
uniform sampler2D image;
in vec3 localPos, worldPos;

vec3 sampleSky(vec3 n, float lod) {
	vec2 p = -n.xz / abs(n.y); // R^2
	float d = length(p);
	p = normalize(p) * (atan(d) / (3.1415926535/2.)); // D^2
	p.x /= 1.6 / 0.9;
	return pow(textureLod(image, p*0.5+0.5, lod).xyz, vec3(2.2));
}

void main(void) {
	vec3 n = normalize(localPos);
	vec3 viewDir = normalize(worldPos - cameraPos);
	vec3 c = vec3(1,0,0);
	// Rim
	vec3 refl = reflect(viewDir, n);
	c += smoothstep(0.0, 1.0, sampleSky(refl, 0)) * 0.3 * (1 - pow(max(0, dot(viewDir, -n)), 1.3));
	// Diffuse
	c *= mix(mix(sampleSky(n, 8), sampleSky(n, 6), 0.25), vec3(1.0), 0.2);
	// Shadow
	c *= 0.8+0.2*n.z;

	c = pow(c, vec3(1/2.2));
	gl_FragColor = vec4(c,1);
}