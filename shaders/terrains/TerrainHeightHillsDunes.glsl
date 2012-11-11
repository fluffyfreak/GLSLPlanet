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
	float continents = ridged_octavenoise(octaves[3], 0.65, lacunarity[3], p, 1.0, 1.0) * (1.0-seaLevel) - (seaLevel*0.1);
	if (continents < 0.0) return 0.0;
	float n = continents;
	float distrib = dunes_octavenoise(octaves[4], 0.4, lacunarity[4], p, 1.0, 1.0);
	distrib *= distrib * distrib;
	float m = octavenoise(octaves[7], 0.5, lacunarity[7], p, 1.0, 1.0) * dunes_octavenoise(octaves[7], 0.5, lacunarity[7], p, 1.0, 1.0)
		* clamp(0.2-distrib, 0.0, 0.05);
	m += octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0) * dunes_octavenoise(octaves[2], 0.5*octavenoise(octaves[6], 0.5*distrib, lacunarity[6], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0) * clamp(1.0-distrib, 0.0, 0.0005);
	float mountains = ridged_octavenoise(octaves[5], 0.5*distrib, lacunarity[5], p, 1.0, 1.0)
		* octavenoise(octaves[4], 0.5*distrib, lacunarity[4], p, 1.0, 1.0) 
		* octavenoise(octaves[6], 0.5, lacunarity[6], p, 1.0, 1.0) * distrib;
	mountains *= mountains;
	m += mountains;
	// smooth cliffs at shore
	if (continents < 0.01) n += m * continents * 100.0;
	else n += m;
	return (n > 0.0 ? n*maxHeight : 0.0);
}
