// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;
uniform int seed;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

float GetHeight(in vec3 p)
{
	float continents = octavenoise(octaves[0], 0.5, lacunarity[0], p, 1.0, 1.0) - seaLevel;
	if (continents < 0.0) 
		return 0.0;
	// unused variable \\ float mountain_distrib = octavenoise(octaves[1], 0.5, p);
	float mountains = octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0);
	float mountains2 = octavenoise(octaves[3], 0.5, lacunarity[3], p, 1.0, 1.0);
	float hill_distrib = octavenoise(octaves[4], 0.5, lacunarity[4], p, 1.0, 1.0);
	float hills = hill_distrib * amplitude[5] * octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0);
	float hills2 = hill_distrib * amplitude[6] * octavenoise(octaves[6], 0.5, lacunarity[6], p, 1.0, 1.0);



	float n = continents - (amplitude[0]*seaLevel);


	if (n < 0.01) 
		n += megavolcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p) * n * 800.0;
	else 
		n += megavolcano_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p) * 8.0;
	
	// there was stuff here, now it's gone.

	n += -1.0;
	n = (n > 0.0 ? n : 0.0);

	n = n*0.03;

	//n += continents - (amplitude[0]*seaLevel);

	if (n > 0.0) {
		// smooth in hills at shore edges
		if (n < 0.1) n += hills * n * 10.0;
		else n += hills;
		if (n < 0.05) n += hills2 * n * 20.0;
		else n += hills2 ;

		mountains  = octavenoise(octaves[1], 0.5, lacunarity[1], p, 1.0, 1.0) * amplitude[2] * mountains*mountains*mountains;
		mountains2 = octavenoise(octaves[4], 0.5, lacunarity[4], p, 1.0, 1.0) * amplitude[3] * mountains2*mountains2*mountains2;
		if (n > 0.5) n += mountains2 * (n - 0.5) ;
		if (n < 0.2) n += mountains * n * 5.0 ;
		else n += mountains  ;
	}

	n = maxHeight*n;
	return (n > 0.0 ? n : 0.0);
}
