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
	float continents = ridged_octavenoise(octaves[3], 0.65, lacunarity[3], p) * (1.0-seaLevel) - (seaLevel*0.1);
	if (continents < 0) return 0;
	float n = continents;
	float distrib = river_octavenoise(octaves[4], 0.5, lacunarity[4], p);
	float m = 0.5* ridged_octavenoise(octaves[4], 0.55*distrib, lacunarity[4], p);
	m += continents*0.25*ridged_octavenoise(octaves[5], 0.58*distrib, lacunarity[5], p);
	// **
	m += 0.001*ridged_octavenoise(octaves[6], 0.55*distrib*m, lacunarity[6], p);
	// cliffs at shore
	if (continents < 0.01) n += m * continents * 100.0f;
	else n += m;
	// was n -= 0.001*ridged_octavenoise(octaves[6], 0.55*distrib*m, p);
	//n += 0.001*ridged_octavenoise(octaves[6], 0.55*distrib*m, p);
	return (n > 0.0 ? n*maxHeight : 0.0);
}
