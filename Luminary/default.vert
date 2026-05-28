#version 460 core
	
in layout(location=0) vec3 v_Position;
in layout(location=1) vec2 v_BNormal;
in layout(location=2) float v_Mass;
in layout(location=3) vec2 v_Size;
in layout(location=4) float v_Seed;

out vec3 Position;
out vec2 BNormal;
out float Mass;
out vec2 Size;
out float Seed;

void main() {
	Position = v_Position;
	BNormal = v_BNormal;
	Mass = v_Mass;
	Size = v_Size;
	Seed = v_Seed;
}