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
	float continents = octavenoise(octaves[0], 0.5, lacunarity[0], p, 1.0, 1.0) - seaLevel;// + (cliff_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p)*0.5);
	if (continents < 0.0) return 0.0;
	float mountain_distrib = octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0);
	float mountains = ridged_octavenoise(octaves[1], 0.5, lacunarity[1], p, 1.0, 1.0);
	//float rocks = octavenoise(octaves[9], 0.5, p);
	float hill_distrib = octavenoise(octaves[4], 0.5, lacunarity[4], p, 1.0, 1.0);
	float hills = hill_distrib * amplitude[3] * billow_octavenoise(octaves[3], 0.5, lacunarity[3], p, 1.0, 1.0);
	float dunes = hill_distrib * amplitude[5] * dunes_octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0);
	float n = continents * amplitude[0] * 2 ;//+ (cliff_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p)*0.5);
	float m = canyon_normal_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	m += canyon2_normal_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	m += canyon3_ridged_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	m = (n<1 ? n : 1/n ); //sometimes creates some interesting features
	m += canyon_billow_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);
	m += canyon2_ridged_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);
	m += canyon3_normal_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);
	m += canyon_normal_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);
	m += canyon2_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);
	m += canyon3_voronoi_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);
	m += -0.5;
	m = n * 0.5;
	m = (n<0.0 ? 0.0 : n);
	n += m;

	// makes larger dunes at lower altitudes, flat ones at high altitude.
	mountains = mountain_distrib * amplitude[3] * mountains*mountains*mountains;
	// smoothes edges of mountains and places them only above a set altitude
	if (n < 0.1) n += n * 10.0 * hills;
	else n += hills;
	if (n > 0.2) n += dunes * (0.2/n);
	else n += dunes;
	if (n < 0.1) n += n * 10.0 * mountains;
	else n += mountains;

	//rocks = mountain_distrib * amplitude[9] * rocks*rocks*rocks;
	//n += rocks ;

	return (n > 0.0? maxHeight*n : 0.0);
}
