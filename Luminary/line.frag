#version 460 core

in vec4 f_color;
in flat float f_renderbool;

out vec4 daColor;

void main() {
	if(f_renderbool == -1.0f) discard;
	daColor = f_color;
}