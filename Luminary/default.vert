#version 460 core

layout(std430, binding = 0) buffer PositionBuffer{
	vec4 positions[];
};

in layout(location=0) vec2 v_BNormal;
in layout(location=1) float v_Mass;
in layout(location=2) vec2 v_Size;
in layout(location=3) float v_Seed;

uniform int PositionFrameOffset;
uniform int ObjectCount;

out vec3 Position;
out vec2 BNormal;
out float Mass;
out vec2 Size;
out float Seed;

void main() {
	Position = vec3(positions[ObjectCount * PositionFrameOffset + gl_VertexID]); // add offset of frame 
	BNormal = v_BNormal;
	Mass = v_Mass;
	Size = v_Size;
	Seed = v_Seed;
}