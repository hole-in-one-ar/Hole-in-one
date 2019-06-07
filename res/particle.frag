#version 440

uniform vec3 cameraPos;
in vec3 localPos, worldPos;
in vec2 fragSeed;

float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) {
	float hue = rand(fragSeed+7.)*3.1415926535*2.;
	vec3 c = cos(vec3(0,1,-1)*2/3*3.1415926535 + hue) * 0.5 + 0.5;
	c = pow(c, vec3(1/2.2));
	c *= 0.5;
	float d = length(localPos);
	if(d > 0.5) discard;
	gl_FragColor = vec4(c,1);
}