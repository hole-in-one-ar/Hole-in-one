#version 440

in vec2 uv;

uniform sampler2D image;

void main(void) {
	vec3 col = texture(image, uv).rgb;
	gl_FragColor = vec4(col,1);
}