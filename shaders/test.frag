// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform vec3 v0;
uniform vec3 v1;
uniform vec3 v2;
uniform vec3 v3;
uniform float fracStep;

// in patch surface coords, [0,1]
// v[0] to v[3] are the corner vertices
vec3 GetSpherePoint(const float x, const double y) {
	return normalize(v0 + x*(1.0-y)*(v1-v0) + x*y*(v2-v0) + (1.0-x)*y*(v3-v0));
}

void wtf()
{
	float xfrac;
	float yfrac = 0.0;
	for (int y=0; y<32; y++) {
		xfrac = 0.0;
		for (int x=0; x<32; x++) {
			vec3 p = GetSpherePoint(xfrac, yfrac);
			//float height = geosphere->GetHeight(p);
			// *(vts++) = p * (height + 1.0);
			xfrac += fracStep;
		}
		yfrac += fracStep;
	}
}

void main()
{
	float feature_size = 1.0; //smaller numbers give a larger feature size
	float poo = octavenoise(3, 0.95, 2.0, tnorm, feature_size, time)/4.0;
	poo *= ridged_octavenoise(3, 0.95, 2.0, tnorm, feature_size*2.5, time*0.25)/4.0;
	poo += billow_octavenoise(3, 0.575, 2.0, tnorm, feature_size*0.5, time*1.333)/4.0;
	poo += combo_octavenoise(3, 0.7, 2.0, tnorm, feature_size*5.675, time*3.0)/4.0;

	gl_FragColor = vec4(poo,0.0,0.0,1.0);
} 