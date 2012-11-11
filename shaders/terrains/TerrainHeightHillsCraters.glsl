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
	float continents = octavenoise(octaves[0], 0.5, lacunarity[0], p, 1.0) - seaLevel;
	if (continents < 0) return 0.0;
	// == TERRAIN_HILLS_NORMAL except river_octavenoise
	float n = 0.3 * continents;
	float distrib = river_octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0);
	float m = amplitude[1] * river_octavenoise(octaves[1], 0.5*distrib, lacunarity[1], p);
	// cliffs at shore
	if (continents < 0.001) n += m * continents * 1000.0f;
	else n += m;
	n += crater_function(octaves[3], amplitude[3], frequency[3], lacunarity[3], p);
	n += crater_function(octaves[4], amplitude[4], frequency[4], lacunarity[4], p);
	n *= maxHeight;
	return (n > 0.0 ? n : 0.0);
}
