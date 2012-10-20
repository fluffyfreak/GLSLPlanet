
#include "shader_heightmap.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

vec3 v0;
vec3 v1;
vec3 v2;
vec3 v3;
float fracStep;

void setUniforms(const glm::vec3 &v0_, const glm::vec3 &v1_, const glm::vec3 &v2_, const glm::vec3 &v3_, const float fracStep_)
{
	v0 =		v0_;
	v1 =		v1_;
	v2 =		v2_;
	v3 =		v3_;
	fracStep =	fracStep_;
}

//
// Description : Array and textureless GLSL 2D/3D/4D simplex
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float mod289(float x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float permute(float x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float taylorInvSqrt(float r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip)
{
    const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
	const vec3 one3 = vec3(1.0, 1.0, 1.0);
    vec4 p,s;
	vec3 t;

    t = floor( fract (vec3(j) * vec3(ip)) * 7.0f) * ip.z - 1.0f;
    p.w = 1.5 - dot(abs(t), one3);
    s = vec4(lessThan(p, vec4(0.0)));
    t = t + (vec3(s)*2.0f - 1.0f) * s.w;
	p.x = t.x;
	p.y = t.y;
	p.z = t.z;

    return p;
}

float snoise(vec4 v)
{
    const vec4  C = vec4( 0.138196601125011,  // (5 - sqrt(5))/20  G4
                          0.276393202250021,  // 2 * G4
                          0.414589803375032,  // 3 * G4
                          -0.447213595499958); // -1 + 4 * G4

	// (sqrt(5) - 1)/4 = F4, used once below
	#define F4 0.309016994374947451

	// First corner
    vec4 i  = floor(v + dot(v, vec4(F4)) );
    vec4 x0 = v -   i + dot(i, vec4(C.x));

	// Other corners

	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
    vec4 i0;
	const vec3 x0yzw = vec3(x0.y,x0.z,x0.w);
	const vec3 x0zww = vec3(x0.z,x0.w,x0.w);
	const vec3 x0yyz = vec3(x0.y,x0.y,x0.z);
    vec3 isX = step( x0yzw, vec3(x0.x) );
    vec3 isYZ = step( x0zww, x0yyz );
	//  i0.x = dot( isX, vec3( 1.0 ) );
    i0.x = isX.x + isX.y + isX.z;
	const vec3 oneMinisX = 1.0f - isX;
    i0.y = oneMinisX.x;
	i0.z = oneMinisX.y;
	i0.w = oneMinisX.z;
	//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
    i0.y += isYZ.x + isYZ.y;
	const vec2 oneMinisYZ = 1.0f - vec2(isYZ);
    i0.z += oneMinisYZ.x;
	i0.w += oneMinisYZ.y;
    i0.z += isYZ.z;
    i0.w += 1.0 - isYZ.z;

    // i0 now contains the unique values 0,1,2,3 in each channel
    vec4 i3 = clamp( i0, 0.0, 1.0 );
    vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
    vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );

    //  x0 = x0 - 0.0 + 0.0 * C.xxxx
    //  x1 = x0 - i1  + 1.0 * C.xxxx
    //  x2 = x0 - i2  + 2.0 * C.xxxx
    //  x3 = x0 - i3  + 3.0 * C.xxxx
    //  x4 = x0 - 1.0 + 4.0 * C.xxxx
    vec4 x1 = x0 - i1 + vec4(C.x);
    vec4 x2 = x0 - i2 + vec4(C.y);
    vec4 x3 = x0 - i3 + vec4(C.z);
    vec4 x4 = x0 + vec4(C.w);

	// Permutations
    i = mod289(i);
    float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
    vec4 j1 = permute( permute( permute( permute (
            i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))
                                         + i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))
                                + i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))
                       + i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));

	// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
	// 7*7*6 = 294, which is close to the ring size 17*17 = 289.
    vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;

    vec4 p0 = grad4(j0,   ip);
    vec4 p1 = grad4(j1.x, ip);
    vec4 p2 = grad4(j1.y, ip);
    vec4 p3 = grad4(j1.z, ip);
    vec4 p4 = grad4(j1.w, ip);

	// Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    p4 *= taylorInvSqrt(dot(p4,p4));

	// Mix contributions from the five corners
    vec3 m0 = max(0.6f - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0f);
    vec2 m1 = max(0.6f - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0f);
    m0 = m0 * m0;
    m1 = m1 * m1;
    return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
                    + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;
}

float octavenoise(int octaves, float roughness, float lacunarity, vec3 p, float jizm, float time)
{
    float n = 0.0;
    float octaveAmplitude = 1.0/(1.0-pow(roughness,(float(octaves))));
    for (int i = 0; i < octaves; i++) {
        n += octaveAmplitude * snoise(vec4(jizm*p.x, jizm*p.y, jizm*p.z, time));
        octaveAmplitude *= roughness;
        jizm *= lacunarity;
    }
    return (n+1.0)*0.5;
}

float combo_octavenoise(int octaves, float roughness, float lacunarity, vec3 p, float jizm, float time)
{
    float n = 0.0;
    float n1 = 0.0;
    float octaveAmplitude = 1.0/(1.0-pow(roughness,(float(octaves))));
    for (int i = 0; i < octaves; i++) {
        n += octaveAmplitude * snoise(vec4(jizm*p.x, jizm*p.y, jizm*p.z, time));
        octaveAmplitude *= roughness;
        jizm *= lacunarity;
    }
    //ridged noise
    n1 = 1.0 - abs(n);
    n1 *= n1;
    //billow noise
    n1 *= (2.0 * abs(n) - 1.0)+1.0;
    //voronoiscam noise
    n1 *= sqrt(10.0 * abs(n));
    return n1;
}

float ridged_octavenoise(int octaves, float roughness, float lacunarity, vec3 p, float jizm, float time)
{
    float n = 0.0;
    float octaveAmplitude = 1.0/(1.0-pow(roughness,(float(octaves))));
    for (int i = 0; i < octaves; i++) {
        n += octaveAmplitude * snoise(vec4(jizm*p.x, jizm*p.y, jizm*p.z, time));
        octaveAmplitude *= roughness;
        jizm *= lacunarity;
    }
    //ridged noise
    n = 1.0 - abs(n);
    return(n*n);
}

float billow_octavenoise(int octaves, float roughness, float lacunarity, vec3 p, float jizm, float time)
{
    float n = 0.0;
    float octaveAmplitude = 1.0/(1.0-pow(roughness,(float(octaves))));
    for (int i = 0; i < octaves; i++) {
        n += octaveAmplitude * snoise(vec4(jizm*p.x, jizm*p.y, jizm*p.z, time));
        octaveAmplitude *= roughness;
        jizm *= lacunarity;
    }
    //ridged noise
    n = (2.0 * abs(n) - 1.0)+1.0;
    return(n);
}


// in patch surface coords, [0,1]
// v[0] to v[3] are the corner vertices
vec3 GetSpherePoint(const float x, const float y) {
	return normalize(v0 + x*(1.0f-y)*(v1-v0) + x*y*(v2-v0) + (1.0f-x)*y*(v3-v0));
}

float shader_heightmap_frag(const glm::vec2 &gl_FragCoord)
{
	float xfrac = (gl_FragCoord.x-0.5f) * fracStep;
	float yfrac = (gl_FragCoord.y-0.5f) * fracStep;
	vec3 p = GetSpherePoint(xfrac, yfrac);

	//smaller numbers give a larger feature size
	float feature_size = 1.0f;
	float poo = octavenoise(3, 0.95f, 2.0f, p, feature_size, 1.0f)/4.0f;
	poo *= ridged_octavenoise(3, 0.95f, 2.0f, p, feature_size*2.5f, 0.25f)/4.0f;
	poo += billow_octavenoise(3, 0.575f, 2.0f, p, feature_size*0.5f, 1.333f)/4.0f;
	poo += combo_octavenoise(3, 0.7f, 2.0f, p, feature_size*5.675f, 3.0f)/4.0f;

	float height = clamp(0.25f+poo, 0.0f, 1.0f);
	return height;
} 
