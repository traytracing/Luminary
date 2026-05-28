#version 460 core

in flat vec3 Center;
in vec2 f_UV;
in flat vec3 f_BNormal;
in flat float f_Seed;

out vec4 fragColor;


const float  PIXELS_BODY  = 1000.0;
const float  RADIUS       = 0.08;
const float  LIGHT_WIDTH  = 0.02;

const vec4   BODY_COLOR[3] = vec4[3](
    vec4(vec3(0.043, 0.031, 0.118), 1.0),   // core
    vec4(vec3(0.929, 0.871, 0.702), 1.0),   // inner rim
    vec4(vec3(0.125, 0.110, 0.267), 1.0)    // hot edge
);

// ‑‑ Accretion disc (ring) ‑‑
const float  PIXELS_RING      = 1000.0;
const float  ROTATION         = 0;        // radians
const vec2   LIGHT_ORIGIN     = vec2(0.39); // helper for light fall‑off
const float  TIME_SPEED       = 0.20;
const float  DISK_WIDTH       = 0.03; //
const float  RING_PERSPECTIVE = 13.0; //
const bool   SHOULD_DITHER    = true;

const int    N_BAND           = 8;
/*
const vec4   RING_COLOR[N_BAND] = vec4[N_BAND](
    vec4(1.00, 1.00, 235.0 / 255.0, 1.0),
    vec4(1.00, 245.0 / 255.0, 64.0 / 255.0, 1.0),
    vec4(1.00, 184.0 / 255.0, 74.0 / 255.0, 1.0),
    vec4(237.0/ 255.0, 123.0 / 255.0, 57.0 / 255.0, 1.0),
    vec4(189.0 /255.0,  64.0 /255.0, 53.0 / 255.0, 1.0)
);*/

vec3 blackhole8[8] = vec3[](
    vec3(0.043, 0.031, 0.118), // deep space shadow        #0b0820
    vec3(0.125, 0.110, 0.267), // muted indigo             #201c44
    vec3(0.243, 0.133, 0.216), // dark magenta-violet      #3e2237
    vec3(0.459, 0.102, 0.125), // dark wine                #751a20
    vec3(0.757, 0.231, 0.110), // deep burnt red           #c13b1c
    vec3(0.933, 0.482, 0.184), // orange-red               #ee7b2f
    vec3(0.961, 0.749, 0.251), // golden yellow            #f5bf40
    vec3(0.992, 0.992, 0.831)  // pale yellow-white        #fdfdd4
);
vec3 purplehole8[8] = vec3[](
    vec3(0.043, 0.031, 0.118), // deep space shadow        #0b0820
    vec3(0.110, 0.086, 0.251), // deep indigo              #1c163f
    vec3(0.180, 0.106, 0.322), // dark violet              #2e1b52
    vec3(0.353, 0.102, 0.408), // royal purple             #5a1a68
    vec3(0.573, 0.231, 0.506), // strong purple-magenta    #923b81
    vec3(0.725, 0.388, 0.725), // mid lavender             #b964b9
    vec3(0.859, 0.659, 0.871), // pastel lilac             #dba8de
    vec3(0.961, 0.902, 0.992)  // pale lavender-white      #f5e6fd
);
vec3 purpleRedHole8[8] = vec3[](
    vec3(0.945, 0.812, 0.710), // warm dusty glow          #f1cfb5
    vec3(0.827, 0.502, 0.431), // dusty rose highlight     #d2806e
    vec3(0.682, 0.282, 0.314), // muted crimson            #ae4850
    vec3(0.486, 0.153, 0.216), // oxblood red              #7c2737
    vec3(0.314, 0.086, 0.196), // dark wine-red            #501631
    vec3(0.204, 0.090, 0.259), // purple-eggplant          #341740
    vec3(0.110, 0.086, 0.251), // deep indigo              #1c163f
    vec3(0.043, 0.031, 0.118)  // deep void                #0b0820
);

// ‑‑ Fractal noise parameters ‑‑
const float SIZE    = 2.7;
const int   OCTAVES = 8;
float SEED    = 2322.0;

/* If you keep TIME as a constant the disc is static.
   Replace it with a uniform for animation. */
uniform float TIME = 0.0;

/* ──────────────────────────────
              Helpers
   ────────────────────────────── */
float rand(vec2 c)
{
    c  = mod(c, vec2(2.0,1.0)*round(SIZE));
    return fract(sin(dot(c, vec2(12.9898,78.233))) * 15.5453 * SEED);
}

float noise(vec2 c)
{
    vec2 i = floor(c);
    vec2 f = fract(c);

    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float d = rand(i + vec2(0.0, 1.0));
    float e = rand(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (d - a) * u.y * (1.0 - u.x) + (e - b) * u.x * u.y;
}

float fbm(vec2 c)
{
    float v = 0.0;
    float amp = 0.5;
    for(int i = 0; i < OCTAVES; ++i)
    {
        v   += noise(c) * amp;
        c   *= 2.0;
        amp *= 0.5;
    }
    return v;
}

vec2 rotate(vec2 p, float a)
{
    float s = sin(a), c = cos(a);
    p -= 0.5;
    p  = mat2(c,-s,s,c) * p;
    return p + 0.5;
}

bool dither() {
    vec2 cell = floor(gl_FragCoord.xy / 2.7);
    bool dither = mod(cell.x + cell.y, 2.0) < 1.0;
    return dither;
}

/* ──────────────────────────────
     Body (event‑horizon) pass
   ────────────────────────────── */
vec4 bodyColour(vec2 uv)
{
    // pixelise
    //uv = floor(uv * PIXELS_BODY) / PIXELS_BODY;

    float d = distance(uv, vec2(0.5));
    //d *= pow(fbm(uv * SIZE), 0.5);
    vec4  col = BODY_COLOR[0];

    if (d > RADIUS - LIGHT_WIDTH)          col = BODY_COLOR[1];
    if (d > RADIUS - LIGHT_WIDTH * 0.5)    col = BODY_COLOR[2];
    if(d > RADIUS) return vec4(col.rgb, 0);
    float alpha = step(d, RADIUS);
    return vec4(col.rgb, 1);
}

uniform vec3 CameraPosition;

vec4 ringColour(vec2 uvReal)
{
    vec3 dir = normalize(CameraPosition - Center);

    float phi = acos(abs(dot(dir, f_BNormal)));

    vec3 ref = abs(f_BNormal.y) < 0.99 ? vec3(0,1,0) : vec3(1,0,0);

// Local tangent (right) and bitangent (up)
vec3 right = normalize(cross(ref, f_BNormal));
vec3 up    = normalize(cross(f_BNormal, right));

// Project onto the plane perpendicular to bNormal
vec3 proj = normalize(dir - dot(dir, f_BNormal) * f_BNormal);

// Express in local tangent space
float x = dot(proj, right);
float y = dot(proj, up);
    float angle2 = atan(y, x);
    
    vec2 uv = floor(uvReal * PIXELS_RING) / PIXELS_RING;
    //uv = rotate(uv, 3.14/2.0);
    bool dith = dither();
    vec2 uv0 = uv;


    float dWidth = 0.03; // diskwidth
    vec2 origin  = vec2(0.5, 0.5);
    float dist = distance(origin, uv);
    
    if(dot(dir, f_BNormal) < 0) {
        if (uv.y <  clamp(0.54 * sin(phi), 0.0, 0.5)) // change with angle 
    {
        float f = smoothstep(dist, 0.5, 0.2) ; // 2
        uv.y    += f; origin.y -= f;
        dWidth  += smoothstep(dist, 0.5, 0.3)  / (15 *cos(phi)); // 3

    } else if (uv.y > 0.5 + 0.5 * cos(phi)) // then change downwards bound 
    {if(cos(phi) >0.4) discard;
        float f = smoothstep(dist, 0.4, 0.17);
        uv.y    -= f; origin.y += f;
        dWidth  += smoothstep(dist, 0.5, 0.2);
    } else {

       dWidth  += smoothstep(dist, 0.5, 0.3)/(15 ); // 3
    }
    } else{
     if (uv.y > 1.0 - clamp(0.54 * sin(phi), 0.0, 0.5)) // change with angle 
    {
        float f = smoothstep(dist, 0.5, 0.2) ; // 2
        uv.y    -= f; origin.y += f;
        dWidth  += smoothstep(dist, 0.5, 0.3)  / (15 *cos(phi)); // 3
        
    } else if (uv.y < 0.5 - 0.5 * cos(phi)) // then change downwards bound 
    {
        if(cos(phi) >0.4) discard;
        float f = smoothstep(dist, 0.4, 0.17);
        uv.y    += f; origin.y -= f;
        dWidth  += smoothstep(dist, 0.5, 0.2);
        
    } else {
       dWidth  += smoothstep(dist, 0.5, 0.3)/(15 ); // 3
    } }
    

    float vpres = 1 / cos(phi);

    vec2  uvC = uv;

    if(dot(dir, f_BNormal) < 0) {
       uvC.y = (-1 * (uvC.y - 0.5)) * vpres + 0.5; // stretch .y only
    }  else {
        uvC.y = (uvC.y - 0.5) * vpres + 0.5; // stretch .y only

    }

    //rotate disk along disk axis
    uvC = rotate(uvC, -1 * angle2); uvC = rotate(uvC, TIME * TIME_SPEED * 3.0);
    float cD   = distance(uvC, vec2(0.5, 0.5));

    float lightD = 0.3 * distance(uv0 * vec2(1.0, vpres), origin * vec2(1.0, vpres)); // noise1 can change 0.3

    float disc = smoothstep(0.1 - 2 * dWidth, 0.5 - dWidth, cD) * smoothstep(cD - dWidth, cD, 0.4);
    disc *= pow(fbm(uvC * SIZE), 0.5); // noise2

    if (dith || !SHOULD_DITHER) disc *= 1.2;

    float band = clamp(disc + lightD, 0.0, 1.0); // 0 - 1
    

     
     

    vec3 color = purpleRedHole8[int(band * (N_BAND - 1))];
    float alpha = step(0.15, disc);
    
    return vec4(color, alpha);
}


void main() {   
    SEED = f_Seed;
    vec4 body = bodyColour(f_UV);
    vec4 ring = ringColour(f_UV);

    /* “Over” compositing – the ring sits on top of the body */
    float a = ring.a + body.a * (1.0 - ring.a);
    vec3  c = (ring.rgb * ring.a + body.rgb * body.a * (1.0 - ring.a)) / max(a, 1e-5);
    // change 0.07 to size of center in flaot
    
    fragColor = vec4(c, a); if(fragColor.a == 0) discard;
}

