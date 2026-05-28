#version 460 core

// Big quad lying on the X-Z plane (y = 0).
// Size is ±10 000 units – adjust if you want.
const vec3 kQuad[6] = vec3[6](
    vec3(-500.0, 0.0, -500.0),
    vec3( 500.0, 0.0, -500.0),
    vec3(-500.0, 0.0,  500.0),

    vec3(-500.0, 0.0,  500.0),
    vec3( 500.0, 0.0, -500.0),
    vec3( 500.0, 0.0,  500.0)
);

uniform mat4 MVP;   // Model-View-Projection
uniform vec3 camPos;      // eye position in world space

out vec3 worldPos;

void main()
{
    worldPos   = vec3(kQuad[gl_VertexID].x + camPos.x, 0.0, kQuad[gl_VertexID].z + camPos.z);
    gl_Position = MVP * vec4(worldPos, 1.0);
}