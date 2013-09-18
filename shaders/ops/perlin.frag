uniform float height, falloff;
uniform int frequency;
uniform int octaves;
uniform int seed;

float rand2(vec2 coord, int frequency){
    int x = int(coord.x)%frequency;
    int y = int(coord.y)%frequency;
    int n = (x+y*frequency);
    n = (n<<13) ^ n;
    return ((n * (n * n * (15731+seed) + 789221) + 1376312589) & 0x7fffffff) / 3147483648.0;
};

/*
float bicubic_interpolate(float v0, float v1, float v2, float v3, float x){
    float P = (v3-v2) - (v0-v1);
    float Q = (v0-v1) - P;
    float R = v2 - v0;
    float S = v1;

    float y = fract(x);
    return P*pow(y, 3) + Q*pow(y, 2) + R*y + S;
}

*/

float nearest_interpolate(float v0, float v1, float v2, float v3, float x){
    if(round(fract(x)) == 0.0){
        return v1;
    }
    else{
        return v2;
    }
}

float bilinear_interpolate(float v0, float v1, float v2, float v3, float x){
    float n = fract(x);
    return v1+(v2-v1)*n;
}

float bezier_interpolate(float v0, float v1, float v2, float v3, float x){
    float n = fract(x);

    return (
        v1 * pow(1.0-n, 3.0) +
        v1 * 3 * pow(1.0-n, 2.0) * n +
        v2 * 3 * (1-n) * pow(n, 2.0) +
        v2 * pow(n, 3.0)
    );
}

float bicubic_interpolate(float v0, float v1, float v2, float v3, float x){
    float n = fract(x);
    return (
        pow(n, 0) * v1 +
        pow(n, 1) * (-0.5*v0 + 0.5*v2) +
        pow(n, 2) * (v0 - 2.5*v1 + 2.0*v2 - 0.5*v3) +
        pow(n, 3) * (-0.5*v0 + 1.5*v1 - 1.5*v2 + 0.5*v3)
    );
}

#define interpolate bicubic_interpolate

float noise2(vec2 c, int frequency){
    //c = c*frequency;
    float h0, h1, h2, h3, v0, v1, v2, v3;

    h0 = rand2(c+vec2(-1,-1), frequency);
    h1 = rand2(c+vec2(+0,-1), frequency);
    h2 = rand2(c+vec2(+1,-1), frequency);
    h3 = rand2(c+vec2(+2,-1), frequency);
    v0 = interpolate(h0, h1, h2, h3, c.x);
    
    h0 = rand2(c+vec2(-1,+0), frequency);
    h1 = rand2(c+vec2(+0,+0), frequency);
    h2 = rand2(c+vec2(+1,+0), frequency);
    h3 = rand2(c+vec2(+2,+0), frequency);
    v1 = interpolate(h0, h1, h2, h3, c.x);
    
    h0 = rand2(c+vec2(-1,+1), frequency);
    h1 = rand2(c+vec2(+0,+1), frequency);
    h2 = rand2(c+vec2(+1,+1), frequency);
    h3 = rand2(c+vec2(+2,+1), frequency);
    v2 = interpolate(h0, h1, h2, h3, c.x);
    
    h0 = rand2(c+vec2(-1,+2), frequency);
    h1 = rand2(c+vec2(+0,+2), frequency);
    h2 = rand2(c+vec2(+1,+2), frequency);
    h3 = rand2(c+vec2(+2,+2), frequency);
    v3 = interpolate(h0, h1, h2, h3, c.x);

    return interpolate(v0, v1, v2, v3, c.y);
}

float noise2_smooth(vec2 coord, int frequency){
    vec2 c = coord*frequency;

    float a = 1.0/36.0;
    float b = 4.0/36.0;
    float z = 16.0/36.0;

    float sd = 0.1;

    return (
        a*noise2(c-vec2(-1,-1)*sd, frequency) + b*noise2(c-vec2(+0,-1)*sd, frequency) + a*noise2(c-vec2(+1,-1)*sd, frequency) +
        a*noise2(c-vec2(-1,+0)*sd, frequency) + z*noise2(c-vec2(+0,+0)*sd, frequency) + a*noise2(c-vec2(+1,+0)*sd, frequency) +
        a*noise2(c-vec2(-1,+1)*sd, frequency) + b*noise2(c-vec2(+0,+1)*sd, frequency) + a*noise2(c-vec2(+1,+1)*sd, frequency)
    );
}

void main(){
    vec2 uv = gl_TexCoord[0].xy;
    float value = 0.0;
    for(int i=frequency; i<frequency+octaves; i++){
        int freq = int(pow(2, i));
        float amplitude = pow(falloff, i-frequency);
        value += noise2_smooth(uv, freq) * amplitude;
    }
    gl_FragColor = vec4(value, value, value, 1.0) * height;
}
