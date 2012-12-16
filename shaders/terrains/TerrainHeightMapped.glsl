// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

uniform sampler2D texHeightmap;
uniform float planetRadius;
uniform float icyness;

const float PI = 3.141592653589793238462643383279502;
const float HALF_PI = 1.570796326794897;
const float PI_2 = 6.283185307179586;

int imax(in int a, in int b)
{
	return a>b ? a : b;
}

int imin(in int a, in int b)
{
	return a<b ? a : b;
}

int iclamp(in int x, in int lo, in int hi)
{
	return imin( hi, imax(lo,x) );
}

float GetHeight(in vec3 p)
{
    // This is all used for Earth and Earth alone

	float latitude = -asin(p.y);
	if (p.y < -1.0) {
		latitude = -HALF_PI;
	}
	if (p.y > 1.0) {
		latitude = HALF_PI;
	}
	
	float longitude = atan(p.z,p.x);
	float px = (((2048.0-1.0) * (longitude + PI)) / (PI_2));
	float py = ((1024.0-1.0) * (latitude + HALF_PI)) / PI;
	int ix = int(floor(px));
	int iy = int(floor(py));
	ix = iclamp(ix, 0, 2048-1);
	iy = iclamp(iy, 0, 1024-1);
	float dx = px-float(ix);
	float dy = py-float(iy);

	// p0,3 p1,3 p2,3 p3,3
	// p0,2 p1,2 p2,2 p3,2
	// p0,1 p1,1 p2,1 p3,1
	// p0,0 p1,0 p2,0 p3,0
	float map0[4];
	float map1[4];
	float map2[4];
	float map3[4];
	int limiy[4];
	for (int y=-1; y<3; y++) {
		limiy[y+1] = iclamp(iy+y, 0, 1024-1);
	}
	int limix = iclamp(ix+0, 0, 2048-1);
	for (int y=0; y<4; y++) {
		vec2 hmuv = vec2(float(limix), float(limiy[y]));
		map0[y] = texture2D(texHeightmap, hmuv).x;
	}
	limix = iclamp(ix+1, 0, 2048-1);
	for (int y=0; y<4; y++) {
		vec2 hmuv = vec2(float(limix), float(limiy[y]));
		map1[y] = texture2D(texHeightmap, hmuv).x;
	}
	limix = iclamp(ix+2, 0, 2048-1);
	for (int y=0; y<4; y++) {
		vec2 hmuv = vec2(float(limix), float(limiy[y]));
		map2[y] = texture2D(texHeightmap, hmuv).x;
	}
	limix = iclamp(ix+3, 0, 2048-1);
	for (int y=0; y<4; y++) {
		vec2 hmuv = vec2(float(limix), float(limiy[y]));
		map3[y] = texture2D(texHeightmap, hmuv).x;
	}

	float c[4];
	for (int j=0; j<4; j++) {
		float d0 = map0[j] - map1[j];
		float d2 = map2[j] - map1[j];
		float d3 = map3[j] - map1[j];
		float a0 = map1[j];
		float a1 = -(1.0/3.0)*d0 + d2 - (1.0/6.0)*d3;
		float a2 = 0.5*d0 + 0.5*d2;
		float a3 = -(1.0/6.0)*d0 - 0.5*d2 + (1.0/6.0)*d3;
		c[j] = a0 + a1*dx + a2*dx*dx + a3*dx*dx*dx;
	}

	float d0 = c[0] - c[1];
	float d2 = c[2] - c[1];
	float d3 = c[3] - c[1];
	float a0 = c[1];
	float a1 = -(1.0/3.0)*d0 + d2 - (1.0/6.0)*d3;
	float a2 = 0.5*d0 + 0.5*d2;
	float a3 = -(1.0/6.0)*d0 - 0.5*d2 + (1.0/6.0)*d3;
	float v = a0 + a1*dy + a2*dy*dy + a3*dy*dy*dy;

	v = (v<0.0 ? 0.0 : v);
	float h = v;

	//Here's where we add some noise over the heightmap so it doesnt look so boring, we scale by height so values are greater high up
	//large mountainous shapes
	float mountains = h*h*0.001*octavenoise(octaves[3-fracnum], 0.5*octavenoise(octaves[5-fracnum], 0.45, lacunarity[5-fracnum], p, 1.0, 1.0), lacunarity[3-fracnum], p, 1.0, 1.0)
		* ridged_octavenoise(octaves[4-fracnum], 0.475*octavenoise(octaves[5-fracnum], 0.4, lacunarity[5-fracnum], p, 1.0, 1.0), lacunarity[4-fracnum], p, 1.0, 1.0);
	v += mountains;
	//smaller ridged mountains
	if (v < 50.0){
		v += v*v*0.04*ridged_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	} else if (v <100.0){
		v += 100.0*ridged_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	} else {
		v += (100.0/v)*(100.0/v)*(100.0/v)*(100.0/v)*(100.0/v)*
			100.0*ridged_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	}

	//low altitude detail/dunes
	if (v < 10.0) {
		v += 2.0*v*dunes_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0) * octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	} else if (v <50.0) {
		v += 20.0*dunes_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0) * octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	} else {
		v += (50.0/v)*(50.0/v)*(50.0/v)*(50.0/v)*(50.0/v) * 20.0 * dunes_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0) * octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	}
	
	if (v <60.0){
		v += (v-40.0)*billow_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	} else {
		v += (30.0/v)*(30.0/v)*(30.0/v)*20.0*billow_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
	}

	//ridges and bumps
	v += h*0.2*voronoiscam_octavenoise(octaves[5-fracnum], clamp(1.0-(h*0.0002), 0.0, 0.6), lacunarity[5-fracnum], p, 1.0, 1.0)
		* clamp(1.0-(h*0.0006), 0.0, 1.0);
	//polar ice caps with cracks
	if ((icyness*0.5)+(abs(p.y*p.y*p.y*0.38)) > 0.6) {
		h = clamp(1.0-(v*10.0), 0.0, 1.0)*voronoiscam_octavenoise(octaves[5-fracnum], 0.5, lacunarity[5-fracnum], p, 1.0, 1.0);
		h *= h*h*2.0;
		h -= 3.0;
		v += h;
	}

	return v<0.0 ? 0.0 : (v/planetRadius);
}
