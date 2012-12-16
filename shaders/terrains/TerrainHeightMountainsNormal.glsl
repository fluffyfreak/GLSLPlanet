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
	//This is among the most complex of terrains, so I'll use this as an example:
{
	//We need a continental pattern to place our noise onto, the 0.7*ridged_octavnoise..... is important here
	// for making 'broken up' coast lines, as opposed to circular land masses, it will reduce the frequency of our
	// continents depending on the ridged noise value, we subtract sealevel so that sea level will have an effect on the continents size
	float continents = octavenoise(octaves[0], 0.7*ridged_octavenoise(octaves[8], 0.58, lacunarity[8], p, 1.0, 1.0), lacunarity[0], p, 1.0, 1.0) - seaLevel*0.65;
	// if there are no continents on an area, we want it to be sea level
	if (continents < 0.0) return 0.0;
	float n = continents - (amplitude[0]*seaLevel*0.5);
	// we save the height n now as a constant h
	float h = n;
	//We don't want to apply noise to sea level n=0
	if (n > 0.0) {
		//large mountainous shapes
		n += h*0.2*ridged_octavenoise(octaves[7], 0.5*octavenoise(octaves[6], 0.5, lacunarity[6], p, 1.0, 1.0), lacunarity[7], p, 1.0, 1.0);

		// This smoothes edges near the coast, we cant have vertical terrain its not handled correctly.
		if (n < 0.4){
			n += n*1.25*ridged_octavenoise(octaves[6],
				clamp(h*0.00002, 0.3, 0.7)*
				ridged_octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0), lacunarity[6], p, 1.0, 1.0);
		} else {
			n += 0.5*ridged_octavenoise(octaves[6],
				clamp(h*0.00002, 0.3, 0.7)*
				ridged_octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0), lacunarity[6], p, 1.0, 1.0);
		}

		if (n < 0.2){
			n += n*15.0*river_octavenoise(octaves[6],
				clamp(h*0.00002, 0.5, 0.7), lacunarity[6], p, 1.0, 1.0);
		} else {
			n += 3.0*river_octavenoise(octaves[6],
				clamp(h*0.00002, 0.5, 0.7), lacunarity[6], p, 1.0, 1.0);
		}

		if (n < 0.4){
			n += n*billow_octavenoise(octaves[6],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0), lacunarity[6], p, 1.0, 1.0);
		} else {
			n += (0.16/n)*billow_octavenoise(octaves[6],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0), lacunarity[6], p, 1.0, 1.0);
		}

		if (n < 0.2){
			n += n*billow_octavenoise(octaves[5],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0), lacunarity[5], p, 1.0, 1.0);
		} else {
			n += (0.04/n)*billow_octavenoise(octaves[5],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0), lacunarity[5], p, 1.0, 1.0);
		}
		//smaller ridged mountains
		n += n*0.7*ridged_octavenoise(octaves[5],
			0.5*octavenoise(octaves[6], 0.5, lacunarity[6], p, 1.0, 1.0), lacunarity[5], p, 1.0, 1.0);

		n = (n/2.0)+(n*n);

		//jagged surface for mountains
		//This is probably using far too much noise, some of it is just not needed
		// More specifically this: clamp(h*0.0002*octavenoise(octaves[5], 0.5, p),
		//		 0.5*octavenoise(octaves[3], 0.5, p),
		//		 0.5*octavenoise(octaves[3], 0.5, p))
		//should probably be: clamp(h*0.0002*octavenoise(octaves[5], 0.5, p),
		//		 0.1,
		//		 0.5)  But I have no time for testing
		if (n > 0.25) {
			n += (n-0.25)*0.1*octavenoise(octaves[3],
				clamp(h*0.0002*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p, 1.0, 1.0),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p, 1.0, 1.0)), lacunarity[3], p, 1.0, 1.0); //[4]?
		}

		if (n > 0.2 && n <= 0.25) {
			n += (0.25-n)*0.2*ridged_octavenoise(octaves[3],
				clamp(h*0.0002*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p, 1.0, 1.0),
				 0.5*octavenoise(octaves[4], 0.5, lacunarity[4], p, 1.0, 1.0)), lacunarity[3], p, 1.0, 1.0);
		} else if (n > 0.05) {
			n += ((n-0.05)/15.0)*ridged_octavenoise(octaves[3],
				clamp(h*0.0002*octavenoise(octaves[5], 0.5, lacunarity[5], p, 1.0, 1.0),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p, 1.0, 1.0),
				 0.5*octavenoise(octaves[4], 0.5, lacunarity[4], p, 1.0, 1.0)), lacunarity[3], p, 1.0, 1.0);
		}
		n = n*0.2;

		if (n < 0.01){
			n += n*voronoiscam_octavenoise(octaves[3],
				clamp(h*0.00002, 0.5, 0.5), lacunarity[3], p, 1.0, 1.0);
		} else if (n <0.02){
			n += 0.01*voronoiscam_octavenoise(octaves[3],
				clamp(h*0.00002, 0.5, 0.5), lacunarity[3], p, 1.0, 1.0);
		} else {
			n += (0.02/n)*0.01*voronoiscam_octavenoise(octaves[3],
				clamp(h*0.00002, 0.5, 0.5), lacunarity[3], p, 1.0, 1.0);
		}

		if (n < 0.001){
			n += n*3.0*dunes_octavenoise(octaves[2],
				1.0*octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0);
		} else if (n <0.01){
			n += 0.003*dunes_octavenoise(octaves[2],
				1.0*octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0);
		} else {
			n += (0.01/n)*0.003*dunes_octavenoise(octaves[2],
				1.0*octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0);
		}

		if (n < 0.001){
			n += n*0.2*ridged_octavenoise(octaves[1],
				0.5*octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[1], p, 1.0, 1.0);
		} else if (n <0.01){
			n += 0.0002*ridged_octavenoise(octaves[1],
				0.5*octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[1], p, 1.0, 1.0);
		} else {
			n += (0.01/n)*0.0002*ridged_octavenoise(octaves[1],
				0.5*octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[1], p, 1.0, 1.0);
		}

		if (n < 0.1){
			n += n*0.05*dunes_octavenoise(octaves[2],
				n*river_octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0);
		} else if (n <0.2){
			n += 0.005*dunes_octavenoise(octaves[2],
				((n*n*10.0)+(3.0*(n-0.1)))*
				river_octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0);
		} else {
			n += (0.2/n)*0.005*dunes_octavenoise(octaves[2],
				clamp(0.7-(1.0-(5.0*n)), 0.0, 0.7)*
				river_octavenoise(octaves[2], 0.5, lacunarity[2], p, 1.0, 1.0), lacunarity[2], p, 1.0, 1.0);
		}

		//terrain is too mountainous, so we reduce the height
		n *= 0.3;

	}

	n = maxHeight*n;
	return (n > 0.0 ? n : 0.0);
}
