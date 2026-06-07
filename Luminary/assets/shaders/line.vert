#version 460 core

layout(std430, binding = 1) readonly buffer TrailHistoryBuffer {
	vec4 TrailPositions[];
};

uniform mat4 CameraMatrix;

uniform int ObjectCount;
uniform int TrailFrameCount;
uniform int TrailWriteFrame;
uniform int ValidTrailFrameCount;

// Replaces v_color.
uniform vec4 TrailColor;

out vec4 f_color;

void main() {
	int objectID = gl_InstanceID;
	int trailStep = gl_VertexID;

	// trailStep = 0 is newest/current point.
	// trailStep = 1 is one stored frame older.
	// trailStep = ValidTrailFrameCount - 1 is oldest.
	int frameIndex =
		(TrailWriteFrame - trailStep + TrailFrameCount) % TrailFrameCount;

	int ssboIndex = frameIndex * ObjectCount + objectID;

	vec4 worldPos = TrailPositions[ssboIndex];

	// Same idea as:
	// fade = elapsedTime / trailTime;
	// alpha = trailColor.a * (1.0 - fade);
	float fade =
		float(trailStep) /
		float(max(ValidTrailFrameCount - 1, 1));

	float alpha = TrailColor.a * (1.0 - fade);

	f_color = vec4(TrailColor.rgb, alpha);

	gl_Position = CameraMatrix * vec4(worldPos.xyz, 1.0);
}