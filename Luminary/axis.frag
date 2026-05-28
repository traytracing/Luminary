#version 460 core

in float f_Distance;

out vec4 daColor;

void main() {
	vec3 color = vec3(1.0f);// make it changeable later
	float dist = f_Distance - 0.5f;
	dist = 2 * abs(dist);
	daColor = vec4(color, dist);
}