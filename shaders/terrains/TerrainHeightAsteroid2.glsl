// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Banded/Ridged pattern mountainous terrain, could pass for desert
float GetHeight(in vec3 p)
{
	float n = voronoiscam_octavenoise(6, 0.2*octavenoise(2, 0.3, 3.7, p), 
		15.0*octavenoise(2, 0.5, 4.0, p), p) * 0.75*ridged_octavenoise(16*octavenoise(2, 0.275, 3.2, p), 
		0.4*ridged_octavenoise(4, 0.4, 3.0, p), 
		4.0*octavenoise(3, 0.35, 3.7, p), p);

	return (n > 0.0? maxHeight*n : 0.0);
}
