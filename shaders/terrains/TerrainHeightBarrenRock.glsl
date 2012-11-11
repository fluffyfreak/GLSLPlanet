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
	float n = ridged_octavenoise(16, 0.5*octavenoise(8, 0.4, 2.5, p), Clamp(5.0*octavenoise(8, 0.257, 4.0, p), 1.0, 5.0), p);
	n = maxHeight*2.0*n*n;

	return (n > 0.0? n : 0.0);
}
