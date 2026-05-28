#version 460 core
// full-screen single triangle
const vec2 POS[3] = vec2[3]( vec2(-1.0,-1.0),
                             vec2( 3.0,-1.0),
                             vec2(-1.0, 3.0) );

out vec4 vWorldPos;               // world position *before* w-divide

uniform mat4 ICameraMatrix;        // inverse( projection * view )

void main()
{
    gl_Position = vec4(POS[gl_VertexID], 0.0, 1.0);   // screen-filling triangle

    vec4 clip = vec4(POS[gl_VertexID], 1.0, 1.0);     // z = w  ⇒ far plane
    vWorldPos = ICameraMatrix * clip;                  // still in homogeneous
}
