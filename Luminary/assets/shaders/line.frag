#version 460 core

in vec4 f_color;

out vec4 daColor;

void main() {
	if (f_color.a <= 0.001) {
		discard;
	}

	daColor = f_color;
}