// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Example used for the animated Star surface in s20dan's ShaderFun branch
float GetHeight(in vec3 p)
{
	//smaller numbers give a larger feature size
	float feature_size = 1.0;
	float poo = octavenoise(3, 0.95, 2.0, p, feature_size, 1.0)*0.25;
	poo += ridged_octavenoise(3, 0.95, 2.0, p, feature_size*2.5, 0.25)*0.25;
	poo += billow_octavenoise(3, 0.575, 2.0, p, feature_size*0.5, 1.333)*0.25;
	poo += combo_octavenoise(3, 0.7, 2.0, p, feature_size*5.675, 3.0)*0.25;

	// float height = poo * maxHeight;
	float height = clamp(poo * maxHeight, 0.0, 1.0);
	return height;
}
