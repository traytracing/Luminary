#version 460 core

in layout(location=0) vec3 v_Position;
in layout(location=1) vec2 v_BNormal;


out vec3 Position;
out vec2 BNormal;

void main() {
	Position = v_Position;
	BNormal = v_BNormal;
}