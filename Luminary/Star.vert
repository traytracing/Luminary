#version 460 core

in layout (location = 0) vec2 v_Angle;
in layout (location = 1) float v_Rotation;
in layout (location = 2) float v_Size;
in layout (location = 3) uint v_StarType;
in layout (location = 4) uint v_StarIndex;

out vec3 Position;
out float Rotation;
out float Size;
out uint StarType;
out uint StarIndex;

void main() {
	Position = vec3(sin(v_Angle.x) * cos(v_Angle.y), sin(v_Angle.x) * sin(v_Angle.y), cos(v_Angle.x));
	Rotation = v_Rotation;
	Size = v_Size;
	StarType = v_StarType;
	StarIndex = v_StarIndex;
} // brighter bigger