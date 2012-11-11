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
	float continents = octavenoise(octaves[0], 0.5, lacunarity[0], p) - seaLevel;
	if (continents < 0) return 0.0;
	// unused variable \\ float mountain_distrib = octavenoise(octaves[1], 0.5, p);
	float mountains = octavenoise(octaves[2], 0.5, lacunarity[2], p);
	float mountains2 = ridged_octavenoise(octaves[3], 0.5, lacunarity[3], p);

	float hill_distrib = octavenoise(octaves[4], 0.5, lacunarity[4], p);
	float hills = hill_distrib * amplitude[5] * ridged_octavenoise(octaves[5], 0.5, lacunarity[5], p);
	float hills2 = hill_distrib * amplitude[6] * octavenoise(octaves[6], 0.5, lacunarity[6], p);

	float hill2_distrib = octavenoise(octaves[7], 0.5, lacunarity[7], p);
	float hills3 = hill2_distrib * amplitude[8] * ridged_octavenoise(octaves[8], 0.5, lacunarity[8], p);
	float hills4 = hill2_distrib * amplitude[9] * ridged_octavenoise(octaves[9], 0.5, lacunarity[9], p);

	float n = continents - (amplitude[0]*seaLevel);

	if (n > 0.0) {
		// smooth in hills at shore edges
		if (n < 0.1) n += hills * n * 10.0f;
		else n += hills;
		if (n < 0.05) n += hills2 * n * 20.0f;
		else n += hills2 ;

		if (n < 0.1) n += hills3 * n * 10.0f;
		else n += hills3;
		if (n < 0.05) n += hills4 * n * 20.0f;
		else n += hills4 ;

		mountains  = octavenoise(octaves[1], 0.5, lacunarity[1], p) *
			amplitude[2] * mountains*mountains*mountains;
		mountains2 = octavenoise(octaves[4], 0.5, lacunarity[4], p) *
			amplitude[3] * mountains2*mountains2*mountains2*mountains2;
		if (n > 0.2) n += mountains2 * (n - 0.2) ;
		if (n < 0.2) n += mountains * n * 5.0f ;
		else n += mountains  ;
	}

	n = maxHeight*n;
	return (n > 0.0 ? n : 0.0);
}
