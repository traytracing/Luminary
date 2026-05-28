#version 460 core

in vec2 f_UV;
in flat uint f_StarType;
in flat uint f_StarIndex;

uniform uint starTypeCount[2]; 
uniform sampler2D starSamplers[2];

out vec4 daColor;


vec3 newP[8] = vec3[](
    vec3(0.106, 0.110, 0.173), // #1b1c2c
    vec3(0.353, 0.235, 0.286), // #5a3c49
    vec3(0.604, 0.392, 0.345), // #9a6458
    vec3(0.839, 0.635, 0.451), // #d6a273
    vec3(0.929, 0.871, 0.702), // #eddeb3
    vec3(0.612, 0.678, 0.718), // #9cadb7
    vec3(0.294, 0.357, 0.671), // #4b5bab
    vec3(0.165, 0.184, 0.290)  // #2a2f4a
);

float starbrightness = 2.0;

void main() { // might want to import as grayscale
    vec4 starGrayColor = texture(starSamplers[f_StarType], vec2((f_UV.x + f_StarIndex) / starTypeCount[f_StarType], f_UV.y));
    int brightness = int (clamp(floor(starGrayColor.x * 7.0), 0.0, 7.0));
	daColor = vec4(starbrightness * newP[brightness], starGrayColor.a);
}