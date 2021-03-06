// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

float GetHeight(in vec3 p)
{
	float continents = octavenoise(octaves[0], 0.5, lacunarity[0], p, 1.0, 1.0) - seaLevel;
	if (continents < 0.0) return 0.0;
	float n = 0.3 * continents;
	float m = amplitude[1] * ridged_octavenoise(octaves[1], 0.5, lacunarity[1], p, 1.0, 1.0);
	float distrib = ridged_octavenoise(octaves[4], 0.5, lacunarity[4], p, 1.0, 1.0);
	if (distrib > 0.5) m += 2.0 * (distrib-0.5) * amplitude[3] * ridged_octavenoise(octaves[3], 0.5*distrib, lacunarity[3], p, 1.0, 1.0);
	// cliffs at shore
	if (continents < 0.001) n += m * continents * 1000.0;
	else n += m;
	n += crater_function(octaves[5], amplitude[5], frequency[5], lacunarity[5], p);
	n += crater_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	n *= maxHeight;
	return (n > 0.0 ? n : 0.0);
}
