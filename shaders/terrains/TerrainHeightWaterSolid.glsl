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
	float continents = 0.7*river_octavenoise(octaves[2], 0.5, lacunarity[2], p)-seaLevel;
	continents = amplitude[0] * ridged_octavenoise(octaves[0],
		Clamp(continents, 0.0, 0.6), lacunarity[0], p);
	float mountains = ridged_octavenoise(octaves[2], 0.5, lacunarity[2], p);
	float hills = octavenoise(octaves[2], 0.5, lacunarity[2], p) *
		amplitude[1] * river_octavenoise(octaves[1], 0.5, lacunarity[1], p);
	float n = continents - (amplitude[0]*seaLevel);
	// craters
	n += crater_function(octaves[5], amplitude[5], frequency[5], lacunarity[5], p);
	if (n > 0.0) {
		// smooth in hills at shore edges
		if (n < 0.05) {
			n += hills * n * 4.0 ;
			n += n * 20.0 * (billow_octavenoise(octaves[3], 0.5*
				ridged_octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[3], p) +
				river_octavenoise(octaves[4], 0.5*
				ridged_octavenoise(octaves[3], 0.5, lacunarity[3], p), lacunarity[4], p) +
				billow_octavenoise(octaves[3], 0.6*
				ridged_octavenoise(octaves[4], 0.55, lacunarity[4], p), lacunarity[3], p));
		} else {
			n += hills * 0.2f ;
			n += billow_octavenoise(octaves[3], 0.5*
				ridged_octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[3], p) +
				river_octavenoise(octaves[4], 0.5*
				ridged_octavenoise(octaves[3], 0.5, lacunarity[3], p), lacunarity[4], p) +
				billow_octavenoise(octaves[3], 0.6*
				ridged_octavenoise(octaves[4], 0.55, lacunarity[4], p), lacunarity[3], p);
		}
		// adds mountains hills craters
		mountains = octavenoise(octaves[3], 0.5, lacunarity[3], p) *
			amplitude[2] * mountains*mountains*mountains;
		if (n < 0.4) n += 2.0 * n * mountains;
		else n += mountains * 0.8f;
	}
	n = maxHeight*n;
	n = (n<0.0 ? -n : n);
	n = (n>1.0 ? 2.0-n : n);
	return n;
}
