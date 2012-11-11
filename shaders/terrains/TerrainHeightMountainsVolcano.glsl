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
	float mountains = octavenoise(octaves[2], lacunarity[2], 0.5, p);
	float mountains2 = octavenoise(octaves[3], lacunarity[3], 0.5, p);
	float hill_distrib = octavenoise(octaves[4], 0.5, lacunarity[4], p);
	float hills = hill_distrib * amplitude[5] * octavenoise(octaves[5], 0.5, lacunarity[5], p);
	float hills2 = hill_distrib * amplitude[6] * octavenoise(octaves[6], 0.5, lacunarity[6], p);



	float n = continents - (amplitude[0]*seaLevel);


	if (n < 0.01) n += megavolcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p) * n * 3000.0f;
	else n += megavolcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p) * 30.0f;

	n = (n > 0.0 ? n : 0.0);

	if ((m_seed>>2)%3 > 2) {
		if (n < 0.2f) n += canyon3_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * n * 2;
		else if (n < 0.4f) n += canyon3_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * 0.4;
		else n += canyon3_ridged_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * (.4/n) * 0.4;
	} else if ((m_seed>>2)%3 > 1) {
		if (n < 0.2f) n += canyon3_billow_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * n * 2;
		else if (n < 0.4f) n += canyon3_billow_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * 0.4;
		else n += canyon3_billow_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * (.4/n) * 0.4;
	} else {
		if (n < 0.2f) n += canyon3_voronoi_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * n * 2;
		else if (n < 0.4f) n += canyon3_voronoi_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * 0.4;
		else n += canyon3_voronoi_function(octaves[8], amplitude[8], frequency[8], lacunarity[8], p) * (.4/n) * 0.4;
	}

	n += -0.05f;
	n = (n > 0.0 ? n : 0.0);

	n = n*0.01f;

	if (n > 0.0) {
		// smooth in hills at shore edges
		if (n < 0.01) n += hills * n * 100.0f;
		else n += hills;
		if (n < 0.02) n += hills2 * n * 50.0f;
		else n += hills2 * (0.02f/n);

		mountains  = octavenoise(octaves[1], 0.5, lacunarity[1], p) *
			amplitude[2] * mountains*mountains*mountains;
		mountains2 = octavenoise(octaves[4], 0.5, lacunarity[4], p) *
			amplitude[3] * mountains2*mountains2*mountains2;
		if (n > 2.5) n += mountains2 * (n - 2.5) * 0.6f;
		if (n < 0.01) n += mountains * n * 60.0f ;
		else n += mountains * 0.6f ;
	}

	n = maxHeight*n;
	return (n > 0.0 ? n : 0.0);
}
