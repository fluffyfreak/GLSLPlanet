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
	float continents = river_octavenoise(octaves[3], 0.65, lacunarity[3], p, 1.0, 1.0) * (1.0-seaLevel) - (seaLevel*0.1);
	if (continents < 0.0) return 0.0;
	float n = continents;
	float distrib = voronoiscam_octavenoise(octaves[4], 0.5*amplitude[5], lacunarity[4], p, 1.0, 1.0);
	float m = 0.1 * amplitude[4] * river_octavenoise(octaves[5], 0.5*distrib, lacunarity[5], p, 1.0, 1.0);
	float mountains = ridged_octavenoise(octaves[5], 0.5*distrib, lacunarity[5], p, 1.0, 1.0) * billow_octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0) *
		voronoiscam_octavenoise(octaves[4], 0.5*distrib, lacunarity[4], p, 1.0, 1.0) * distrib;
	m += mountains;
	//detail for mountains, stops them looking smooth.
	m += mountains*mountains*0.02*ridged_octavenoise(octaves[2], 0.6*mountains*mountains*distrib, lacunarity[2], p, 1.0, 1.0);
	m *= m*m*m*10.0;
	// smooth cliffs at shore
	if (continents < 0.01) n += m * continents * 100.0;
	else n += m;
	n += continents*clamp(0.5-m, 0.0, 0.5)*0.2*river_octavenoise(octaves[6], 0.6*distrib, lacunarity[6], p, 1.0, 1.0);
	n += continents*clamp(0.05-n, 0.0, 0.01)*0.2*dunes_octavenoise(octaves[2], clamp(0.5-n, 0.0, 0.5), lacunarity[2], p, 1.0, 1.0);
	n *= maxHeight;
	return (n > 0.0 ? n : 0.0);
}
