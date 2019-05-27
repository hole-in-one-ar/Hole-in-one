in vec3 v;
uniform float time;

void main(void) {
	vec3 c = v;
	gl_FragColor = vec4(c*0.5+0.5,1);
}