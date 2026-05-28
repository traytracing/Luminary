#version 460 core

//change later + naming convention and 
float size = 15.0;
int OCTAVES = 8;
const float seed = 4.175;
const bool reduce_background = false;

// add const to dither
bool dither = mod(gl_FragCoord.x + gl_FragCoord.y, 2.0) < 1.0; // checker board pattern in dust | see if(dith)


float rand(vec3 dir) {
	return fract(sin(dot(dir, vec3(12.9898, 78.233, 37.719))) * (15.5453 + seed));
}

float noise(vec3 dir) {
    vec3 i = floor(dir);
    vec3 f = fract(dir);

    float a = rand(i);
    float b = rand(i + vec3(1,0,0));
    float c = rand(i + vec3(0,1,0));
    float d = rand(i + vec3(1,1,0));
    float e = rand(i + vec3(0,0,1));
    float f1= rand(i + vec3(1,0,1));
    float g = rand(i + vec3(0,1,1));
    float h = rand(i + vec3(1,1,1));

    vec3 u = f * f * (3.0 - 2.0 * f);

    return mix(
             mix(mix(a,b,u.x), mix(c,d,u.x), u.y),
             mix(mix(e,f1,u.x),mix(g,h,u.x), u.y),
             u.z);
}


float fbm3D(vec3 dir, int oct) {
    float value = 0.0;
    float amp   = 0.5;

    for (int i = 0; i < oct; ++i)
    {
        value += noise(dir) * amp;
        dir *= 2.0;
        amp *= 0.5;
    }
    return value;
}

float circleNoise3D(vec3 dir) {
    float dir_z = floor(dir.z);
    dir.xy += dir_z * vec2(0.31, 0.37);
    vec3 cell = floor(dir);
    vec3 f    = fract(dir);
    float h = rand(cell);
    float m = length(f - vec3(0.25) - h * 0.5);
    float r = h * 0.25;
    
    return smoothstep(0.0, r, m * 0.75);
}

float cloudAlpha3D(vec3 dir, int oct, int iter) {
    float acc = 0.0;
    for (int i = 0; i < iter; ++i)
    {
        acc += circleNoise3D(dir * 0.5 + float(i + 1) + vec3(-0.3, 0.0, 0.0));
    }
    return fbm3D(dir + acc, oct);
}



const float brightness = 56.0f;

// finish this later NOT DONE
vec4 spaceDust(vec3 dir) {
    // noise
    size = 30;
    int iter = 5;
	float n_dust_circle = cloudAlpha3D(dir, OCTAVES, iter); // circle-like noise 
	float n_dust_blob = fbm3D(dir * size * 0.2  - vec3(2.0), OCTAVES); // blob-like noise
	float n_dust_alpha = fbm3D(dir * size * 0.5 + vec3(2.0), OCTAVES);
	
    float dust_value = n_dust_circle * n_dust_blob;


	// dithering | noise modifications 
	if (dither) dust_value *= 0.95;
	
	dust_value = pow(dust_value, 3.2) * brightness; // put under a_dust if it doesnt look good
	float dust_alpha = step(n_dust_alpha, dust_value * 1.8);
	
	if (dither) dust_value *= 1.1; 
	
	

	// coloring
	if (reduce_background) dust_value = pow(dust_value, 0.8) * 0.7;
	
	float normalized_color_level = floor(dust_value) / 7.0;
	
	vec3 col = vec3(normalized_color_level); // lookup color later
	
    return vec4(col, dust_alpha);
}

// finish this later NOT DONE
vec4 nebula(vec3 dir) {
	// noise
	OCTAVES = 3;
    size = 15;
    int iter = 2;
	float n_dust_circle = cloudAlpha3D(dir * size, OCTAVES, iter);
	float n_dust_circle_2 = cloudAlpha3D(dir * size, OCTAVES, iter);
	float n_dust_blob = fbm3D(dir * size + vec3(1.0), OCTAVES);

	float circle_value = n_dust_circle * n_dust_blob;
	float dust_value = n_dust_circle_2 * circle_value;

	// dithering | noise modifications 
	float dist = 0.25; // acos(dot(dir, nebulaCenter)) / 6.14159 const vec3 nebulaCenter = normalize(vec3(0.0, 0.0, -1.0));
	//if(dist > 1.14159) return vec4(0);

	if (dither) {
		dust_value *= 0.95;
		circle_value *= 0.95;
		dist*= 0.98;
	}

	// slightly offset alpha values to create thin bands around the nebulae
	float a = step(n_dust_blob, 0.1 + dist);
	float a2 = step(n_dust_blob, 0.115 + dist);

	// choose colors
	if (reduce_background) {
		dust_value = pow(dust_value, 1.2) * 0.7;
	}
	float col_value = 0.0;
	if (a2 > a) {
		col_value = floor(dust_value * 35.0) / 7.0;
	} else {
		col_value = floor(dust_value * 14.0) / 7.0;
	}
	
	// apply colors
	vec3 col = vec3(col_value);
	if (col_value < 0.1) {
		col = vec3(0.0901961, 0.0901961, 0.0666667); // fix later
	}

    return vec4(col, a2);
}

in vec2 f_UV;
in vec4 vWorldPos;

out vec4 daColor;

float hash12(vec2 p){  // returns 0-1
    vec3 p3 = fract(vec3(p.xyx)*.1031);
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.x+p3.y)*p3.z);
}
vec2  hash22(vec2 p){ return vec2(hash12(p), hash12(p+13.37)); }

// --------- star field --------------------------
float starFieldAA(vec3 dir)
{
    // map direction to a 2-D plane (any low-distortion mapping is fine)
    vec2 uv = dir.xy / (abs(dir.z)+1.0);

    const float SCALE = 800.0;                 // density control
    vec2  gpos  = uv * SCALE;                  // grid coords
    vec2  cell  = floor(gpos);

    // pick the ~0.3 % brightest cells
    if(hash12(cell) < 0.997) return 0.0;

    // random centre *inside* the cell so stars don’t sit on grid lines
    vec2  centre = hash22(cell) - 0.5;
    vec2  dp     = fract(gpos) - centre;       // vector from star centre
    float d      = length(dp);

    // radius in “cell units” (≈ pixels at this scale)
    const float R = 0.35 * rand(dir);

    // screen-space derivative = one-pixel width → smooth cutoff
    float w = fwidth(d);                       // always ≥ one pixel
    return smoothstep(R + w, R - w, d);        // 0-1 intensity
}

vec3 nyx8[8] = vec3[](
    vec3(0.031, 0.078, 0.118), // #08141e
    vec3(0.059, 0.165, 0.247), // #0f2a3f
    vec3(0.126, 0.224, 0.310), // #20394f
    vec3(0.965, 0.839, 0.741), // #f6d6bd
    vec3(0.765, 0.639, 0.541), // #c3a38a
    vec3(0.600, 0.459, 0.467), // #997577
    vec3(0.506, 0.384, 0.443), // #816271
    vec3(0.306, 0.286, 0.373)  // #4e495f
);

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


void main()
{
    vec3 worldPos = vWorldPos.xyz / vWorldPos.w;
    vec3 dir      = normalize(worldPos);

    vec4 dust = spaceDust(dir);
    vec4 neb  = nebula(dir);

    float  aOut = neb.a + dust.a * (1.0 - neb.a);
    vec3   cOut = neb.rgb * neb.a + dust.rgb * (1.0 - neb.a);
     
    int cOutQuant = int (clamp(floor(cOut.x * 7.0), 0.0, 7.0) );
    cOut = newP[cOutQuant];

    vec3 stars = vec3(starFieldAA(dir));
    cOut += stars;

    daColor = vec4(cOut, aOut);

}


/*
float bayerDither(vec2 uv) {
    int x = int(mod(uv.x, 4.0));
    int y = int(mod(uv.y, 4.0));
    int index = y * 4 + x;
    float threshold = float[16](
         0.0,  8.0,  2.0, 10.0,
        12.0,  4.0, 14.0,  6.0,
         3.0, 11.0,  1.0,  9.0,
        15.0,  7.0, 13.0,  5.0
    )[index] / 16.0;
    return threshold;
}
*/