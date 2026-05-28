#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 Position[];
in float Rotation[];
in float Size[];
in uint StarType[];
in uint StarIndex[];

uniform mat4 camMatrix;
uniform vec3 camPos;

out vec2 f_UV;
out flat uint f_StarType;
out flat uint f_StarIndex;

void main() {
    f_StarType = StarType[0];
    f_StarIndex = StarIndex[0];

    vec3 center = Position[0];

    // Face direction from particle to camera
    vec3 dir = normalize(camPos - center);

    // World-up axis
    vec3 up = vec3(0.0, 1.0, 0.0);

    // Orthonormal basis for billboard orientation
    vec3 right = normalize(cross(up, dir));
    vec3 billboardUp = normalize(cross(dir, right));

    float angle = Rotation[0];
    float c = cos(angle);
    float s = sin(angle);

    // 2D rotation in the plane perpendicular to `dir`
    vec3 rRot = c * right + s * billboardUp;
    vec3 uRot = -s * right + c * billboardUp;

    
    // Compute world-space quad corners
    vec3 p1 = center - rRot * Size[0] - uRot * Size[0]; // bottom-left
    vec3 p2 = center + rRot * Size[0] - uRot * Size[0]; // bottom-right
    vec3 p3 = center - rRot * Size[0] + uRot * Size[0]; // top-left
    vec3 p4 = center + rRot * Size[0] + uRot * Size[0]; // top-right

    // Emit the quad in triangle strip order

    gl_Position = camMatrix * vec4(p1, 1.0);
    f_UV = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = camMatrix * vec4(p2, 1.0);
    f_UV = vec2(1.0 , 0.0);
    EmitVertex();

    gl_Position = camMatrix * vec4(p3, 1.0);
    f_UV = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = camMatrix * vec4(p4, 1.0);
    f_UV = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
