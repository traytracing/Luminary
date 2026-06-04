#version 460 core
in vec3 worldPos;
uniform vec3 camPos;      // eye position in world space

out vec4 FragColor;

//-------------------------------------------------
//  Antialiased line mask. size = grid spacing.
// ---- helpers ---------------------------------------------------------------
float gridMask(vec2 pos, float size)
{
    vec2  g    = abs(fract(pos / size - 0.5) - 0.5) / fwidth(pos / size);
    float line = min(g.x, g.y);
    return 1.0 - clamp(line, 0.0, 1.0);      // 2-D antialiased grid cell
}

float axisMask(float v)                       // 1-D antialiased axis line
{
    float w = fwidth(v);
    return 1.0 - clamp(abs(v) / w, 0.0, 1.0);
}

// ---- main ------------------------------------------------------------------
void main()
{
    //----------------------------------------------------------------- grid --
    vec2 coord = worldPos.xz;                 // XZ plane, Y-up
    float minor = gridMask(coord,  1.0);
    float major = gridMask(coord, 10.0);
    float grid  = max(major, minor * 0.4);    // mix major + faint minor

    //---------------------------------------------------------------- distance–
    float dist  = distance(worldPos, camPos); // fade with distance
    float fade  = clamp(1.0 - dist / 200.0, 0.0, 1.0);

    //---------------------------------------------------------------- colours –
    vec3  base  = vec3(0.30);                 // dark background
    vec3  line  = vec3(0.80);                 // grey grid lines
    vec3  colour= mix(base, line, grid);      // start with grid

    //---------------------------------------------------------------- axes ----
    float xMask = axisMask(coord.y) * step(0.0, coord.x); // +X axis only
    float zMask = axisMask(coord.x) * step(0.0, coord.y); // +Z axis only

    colour = mix(colour, vec3(1,0,0), xMask);             // red X
    colour = mix(colour, vec3(0,0,1), zMask);             // blue Z

    float alpha = max(grid, max(xMask, zMask)) * fade;    // premultiplied

    FragColor = vec4(colour * fade, alpha);
}

