#version 460 core

in layout (location = 0) vec3 v_position;
in layout (location = 1) float v_renderbool;
in layout (location = 2) vec4 v_color;

uniform mat4 CameraMatrix;

out vec4 f_color;
out flat float f_renderbool;

void main() {
	f_color = v_color;
	f_renderbool = v_renderbool;
	gl_Position = CameraMatrix * vec4(v_position, 1.0f);
}