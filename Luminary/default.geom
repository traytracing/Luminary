#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 Position[];
in vec2 BNormal[];
in vec2 Size[];
in float Mass[];
in float Seed[];

uniform mat4 CameraMatrix; // change name later
uniform vec3 CameraPosition; // change name later

out flat vec3 Center;
out vec2 f_UV;
out flat vec3 f_BNormal;
out flat float f_Seed;

void main() {
    f_Seed = Seed[0];

    Center = Position[0];
    vec3 dirToCam = normalize(CameraPosition - Center);
    f_BNormal = vec3(sin(BNormal[0].x) * cos(BNormal[0].y), sin(BNormal[0].x) * sin(BNormal[0].y), cos(BNormal[0].x)); //Reconstructing the BNormal in 3D
    
    vec3 up = f_BNormal;
    vec3 right = normalize(cross(up, dirToCam));
    vec3 billboardUp = normalize(cross(dirToCam, right));

    vec2 size = Mass[0] * Size[0];

    vec3 p1 = Center - right * size.x - billboardUp * size.y; // Bottom-left
    vec3 p2 = Center + right * size.x - billboardUp * size.y; // Bottom-right
    vec3 p3 = Center - right * size.x + billboardUp * size.y; // Top-left
    vec3 p4 = Center + right * size.x + billboardUp * size.y; // Top-right

    gl_Position = CameraMatrix * vec4(p1, 1.0f); f_UV = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = CameraMatrix * vec4(p2, 1.0f); f_UV = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = CameraMatrix * vec4(p3, 1.0f); f_UV = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = CameraMatrix * vec4(p4, 1.0f); f_UV = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
