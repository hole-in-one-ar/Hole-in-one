#version 440

uniform vec3 cameraPos;
uniform sampler2D digit;
uniform float thickness;
uniform float color;
uniform int value;
in vec3 localPos, worldPos;
in vec3 n;

float median(float r, float g, float b) {
	return max(min(r,g), min(max(r,g), b));
}
float renderMSDF(vec2 uv) {
	vec3 s = texture(digit, uv).rgb;
	float sigDist = median(s.r, s.g, s.b) - 0.5;
	return sigDist;
}

float pickCharacter(vec2 uv) {
	float dnum = value == 0 ? 1.f : floor(log(value+0.5) / log(10)) + 1.f;
	uv.x -= 2.0; 
	uv.x += (8 - dnum) / 2.0 * 0.8;

	float c = -1.0;
	float target = value;
	for(int i=0;i<4;i++) {
		float v = floor(mod(target,10));
		vec2 iuv = uv - vec2(3-i,0)*0.8;
		if((i==0 || floor(target) > 0) && 0.02 < iuv.x && iuv.x < 1 - 0.02 && 0 < iuv.y && iuv.y < 32/24.0) {
			c = max(c, renderMSDF(iuv*vec2(24/256.0,24/32.0) + vec2(v*24/256.0,0)));
		}
		target /= 10;
	}
	return c;
}

void main(void) {
	vec3 viewDir = normalize(worldPos - cameraPos);
	vec3 c = vec3(1.5,1.3,0.5);
	c *= smoothstep(0., 1., color) * 0.5 + 0.5;
	vec2 luv = - localPos.xy * 0.5 + 0.5;
	float ch = pickCharacter(luv * vec2(4,2));
	ch -= pow(1. - thickness, 4.0);
	if(ch < -0.4) discard;
	c.gb *= smoothstep(0, 0.05, ch) * 0.5 + 0.5;
	gl_FragColor = vec4(c,1);
}