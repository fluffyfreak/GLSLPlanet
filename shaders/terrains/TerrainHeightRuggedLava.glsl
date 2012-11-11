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
	float continents = octavenoise(octaves[0], Clamp(0.725-(seaLevel/2), 0.1, 0.725), lacunarity[0], p) - seaLevel;
	if (continents < 0.0) return 0;
	float mountain_distrib = octavenoise(octaves[1], 0.55, lacunarity[1], p);
	float mountains = octavenoise(octaves[2], 0.5, lacunarity[2], p) * ridged_octavenoise(octaves[2], 0.575, lacunarity[2], p);
	float mountains2 = octavenoise(octaves[3], 0.5, lacunarity[3], p);
	float hill_distrib = octavenoise(octaves[4], 0.5, lacunarity[4], p);
	float hills = hill_distrib * amplitude[5] * octavenoise(octaves[5], 0.5, lacunarity[5], p);
	float rocks = octavenoise(octaves[9], 0.5, lacunarity[9], p);


	float n = continents - (amplitude[0]*seaLevel);
	n += mountains*mountains2*5.0*megavolcano_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	n += 2.5*megavolcano_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	n += mountains*mountains2*5.0*volcano_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p)*volcano_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);
	n += 2.5*volcano_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p);

	n += mountains*mountains2*7.5*megavolcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);
	n += 2.5*megavolcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);
	n += mountains*mountains2*7.5*volcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p)*volcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);
	n += 2.5*volcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p);

	//smooth canyon transitions and limit height of canyon placement
	if (n < 0.01) n += n * 100.0f * canyon3_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);
	else n += canyon3_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);

	if (n < 0.01) n += n * 100.0f * canyon2_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);
	else n += canyon2_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p);
	n *= 0.5;

	n += continents*hills*hill_distrib*mountain_distrib;

	mountains  = octavenoise(octaves[1], 0.5, lacunarity[1], p) *
			amplitude[2] * mountains*mountains*mountains;
	mountains2 = octavenoise(octaves[4], 0.5, lacunarity[4], p) *
			amplitude[3] * mountains2*mountains2*mountains2;

	n += continents*mountains*hill_distrib ;
	if (n < 0.01) n += continents*mountains2 * n * 40.0f ;
	else n += continents*mountains2*.4f ;
	n *= 0.2;
	n += mountains*mountains2*mountains2*hills*hills*hill_distrib*mountain_distrib*20.0;

	rocks = continents * mountain_distrib * amplitude[9] * rocks*rocks*rocks * 2.0;
	n += rocks;

	n = (n<0.0 ? 0.0 : maxHeight*n);
	return n;
}
