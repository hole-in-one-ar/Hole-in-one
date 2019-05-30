#version 440

in vec3 localPos, worldPos;

void main(void) {
	if(length(localPos.xy) > 0.99) discard;
	vec3 n = normalize(localPos);
	vec3 c = n;
	gl_FragColor = vec4(c*0.5+0.5,1);
}