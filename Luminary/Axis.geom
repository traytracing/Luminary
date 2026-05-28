#version 460 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in vec3 Position[];
in vec2 BNormal[];

uniform mat4 CameraMatrix;
uniform float Scale;

out float f_Distance;

void main() {
	vec3 dirOfAxis = vec3(sin(BNormal[0].x) * cos(BNormal[0].y), sin(BNormal[0].x) * sin(BNormal[0].y), cos(BNormal[0].x));

	gl_Position = CameraMatrix * vec4(Position[0] + Scale * -dirOfAxis, 1.0f);
	f_Distance = 0.0f;
	EmitVertex();

	gl_Position = CameraMatrix * vec4(Position[0] + Scale * dirOfAxis, 1.0f);
	f_Distance = 1.0f;
	EmitVertex();

	EndPrimitive();
}