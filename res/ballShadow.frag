#version 440

in vec3 localPos, worldPos;

void main(void) {
	if(length(localPos.xy) > 1.0) discard;
	gl_FragColor = vec4(0,0,0,0.4);
}