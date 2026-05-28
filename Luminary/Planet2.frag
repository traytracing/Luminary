#version 460 core

//change later + naming convention and 
const vec2 uv_correct = vec2(1.0); // should be size variable and no more pixels + size
const float pixels = 500.0;


const float size = 10.0;
const int OCTAVES = 8;
const float seed = 6.175;
const bool reduce_background = false;

bool dither(vec2 uv1, vec2 uv2) {
	return mod(uv1.y+uv2.x,2.0/pixels) <= 1.0 / pixels;
}

vec2 rotate(vec2 vec, float angle) {
	vec -=vec2(0.5);
	vec *= mat2(vec2(cos(angle),-sin(angle)), vec2(sin(angle),cos(angle)));
	vec += vec2(0.5);
	return vec;
}

float rand(vec2 coord) {
	return fract(sin(dot(coord.xy ,vec2(12.9898,78.233))) * (15.5453 + seed));
}

float noise(vec2 coord){
	vec2 i = floor(coord);
	vec2 f = fract(coord);
		
	float a = rand(i);
	float b = rand(i + vec2(1.0, 0.0));
	float c = rand(i + vec2(0.0, 1.0));
	float d = rand(i + vec2(1.0, 1.0));

	vec2 cubic = f * f * (3.0 - 2.0 * f);

	return mix(a, b, cubic.x) + (c - a) * cubic.y * (1.0 - cubic.x) + (d - b) * cubic.x * cubic.y;
}

float fbm(vec2 coord){
	float value = 0.0;
	float scale = 0.5;

	for(int i = 0; i < OCTAVES ; i++){
		value += noise(coord) * scale;
		coord *= 2.0;
		scale *= 0.5;
	}
	return value;
}

float circleNoise(vec2 uv) {
    float uv_y = floor(uv.y);
    uv.x += uv_y*.31;
    vec2 f = fract(uv);
	float h = rand(vec2(floor(uv.x),floor(uv_y)));
    float m = (length(f-0.25-(h*0.5)));
    float r = h*0.25;
    return smoothstep(0.0, r, m*0.75);
}

float cloud_alpha(vec2 uv) {
	float c_noise = 0.0;
	
	// more iterations for more turbulence
	int iters = 5;
	for (int i = 0; i < iters; i++) {
		c_noise += circleNoise(uv * 0.5 + (float(i+1)) + vec2(-0.3, 0.0));
	}
	float fbm = fbm(uv+c_noise);
	
	return fbm;
}



in vec2 f_UV;

out vec4 daColor;

const float brightness = 56.0f;
void main() {
	vec2 uv = floor((f_UV) * pixels) / pixels * uv_correct; // fix pixels & remove uv_correct
	
	float n_dust_circle = cloud_alpha(uv * size); // circle-like noise 
	float n_dust_blob = fbm(uv * ceil(size * 0.2)  -vec2(2,2)); // blob-like noise
	float n_dust_alpha = fbm(uv * ceil(size * 0.5) +vec2(2,2)); // same ^
	float n_dust_lerp = n_dust_circle * n_dust_blob;



	// dithering | noise modifications 
	bool dith = dither(uv, f_UV); // checker board pattern in dust | see if(dith)
	if (dith) n_dust_lerp *= 0.95; // 0.95 
	
	n_dust_lerp = pow(n_dust_lerp, 3.2) * brightness; // put under a_dust if it doesnt look good
	
	float a_dust = step(n_dust_alpha, n_dust_lerp * 1.8);
	
	if (dith) n_dust_lerp *= 1.1; 
	
	

	// coloring
	if (reduce_background) n_dust_lerp = pow(n_dust_lerp, 0.8) * 0.7;
	
	float normalized_color_level = floor(n_dust_lerp) / 7.0; // normalize
	
	vec3 col = vec3(normalized_color_level); // lookup color later
	
	
	
	daColor = vec4(col, a_dust);
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