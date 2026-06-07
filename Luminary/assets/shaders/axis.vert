#version 460 core

in layout(location=0) vec2 v_BNormal;

layout(std430, binding = 0) readonly buffer PositionBuffer {
    vec4 Positions[];
};

uniform int PositionFrameOffset;
uniform int ObjectCount;

out vec3 Position;
out vec2 BNormal;

void main() {
	int objectID = gl_VertexID;

    vec4 objectPosition =
        Positions[PositionFrameOffset * ObjectCount + objectID];

    Position = objectPosition.xyz;
	BNormal = v_BNormal;
}