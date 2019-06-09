#version 440

uniform vec3 cameraPos;
in vec3 localPos, worldPos;

float sq(vec2 uv) {
	uv = abs(uv);
	return max(uv.x - 0.5, uv.y - 1.5);
}
float tr(vec2 uv) {
	float u = dot(uv, normalize(vec2(2,-1))) - 1.;
	return max(u, uv.y);
}

float df(vec2 uv) {
	uv.x = abs(uv.x);
	return min(sq(uv-vec2(0,0.5)),tr(uv));
}

void main(void) {
	float d = df(localPos.xy*3.);
	if(d > 0.) discard;
	vec3 c = vec3(1,0,0.2);
	c *= smoothstep(-0.1,-0.2,d) * 0.6 + 0.4;
	gl_FragColor = vec4(c,1);
}