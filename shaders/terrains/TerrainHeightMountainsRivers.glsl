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
	float continents = octavenoise(octaves[0], 0.7*ridged_octavenoise(octaves[8], 0.58, lacunarity[8], p), lacunarity[0], p) - seaLevel*0.65;
	if (continents < 0) return 0.0;
	float n = (river_function(octaves[9], amplitude[9], frequency[9], lacunarity[9], p)*
		river_function(octaves[7], amplitude[7], frequency[7], lacunarity[7], p)*
		river_function(octaves[6], amplitude[6], frequency[6], lacunarity[6], p)*
		canyon3_normal_function(octaves[1], amplitude[1], frequency[1], lacunarity[1], p)*continents) -
		(amplitude[0]*seaLevel*0.1);
	n *= 0.5;

	float h = n;

	if (n > 0.0) {
		// smooth in hills at shore edges
		//large mountainous shapes
		n += h*river_octavenoise(octaves[7],
			0.5*octavenoise(octaves[6], 0.5, lacunarity[6], p), lacunarity[7], p);

		//if (n < 0.2) n += canyon3_billow_function(octaves[9], amplitude[9], frequency[9], lacunarity[9], p) * n * 5;
		//else if (n < 0.4) n += canyon3_billow_function(octaves[9], amplitude[9], frequency[9], lacunarity[9], p);
		//else n += canyon3_billow_function(octaves[9], amplitude[9], frequency[9], lacunarity[9], p) * (0.4/n);
		//n += -0.5;
	}

	if (n > 0.0) {
		if (n < 0.4){
			n += n*2.5*river_octavenoise(octaves[6],
				clamp(h*0.00002, 0.3, 0.7)*
				ridged_octavenoise(octaves[5], 0.5, lacunarity[5], p), lacunarity[6], p);
		} else {
			n += 1.0*river_octavenoise(octaves[6],
				clamp(h*0.00002, 0.3, 0.7)*
				ridged_octavenoise(octaves[5], 0.5, lacunarity[5], p), lacunarity[6], p);
		}
	}

	if (n > 0.0) {
		if (n < 0.2){
			n += n*5.0*billow_octavenoise(octaves[6],
				clamp(h*0.00002, 0.5, 0.7), lacunarity[6], p);
		} else {
			n += billow_octavenoise(octaves[6],
				clamp(h*0.00002, 0.5, 0.7), lacunarity[6], p);
		}
	}

	if (n > 0.0) {
		if (n < 0.4){
			n += n*2.0*river_octavenoise(octaves[6],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p), lacunarity[6], p);
		} else {
			n += (0.32/n)*river_octavenoise(octaves[6],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p), lacunarity[6], p);
		}

		if (n < 0.2){
			n += n*ridged_octavenoise(octaves[5],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p), lacunarity[5], p);
		} else {
			n += (0.04/n)*ridged_octavenoise(octaves[5],
				0.5*octavenoise(octaves[5], 0.5, lacunarity[5], p), lacunarity[5], p);
		}
		//smaller ridged mountains
		n += n*0.7*ridged_octavenoise(octaves[5],
			0.7*octavenoise(octaves[6], 0.6, lacunarity[6], p), lacunarity[5], p);

		//n += n*0.7*voronoiscam_octavenoise(octaves[5],
		//	0.7*octavenoise(octaves[6], 0.6, p), p);

		//n = n*0.6667;

		//jagged surface for mountains
		if (n > 0.25) {
			n += (n-0.25)*0.1*octavenoise(octaves[3],
				clamp(h*0.0002*octavenoise(octaves[5], 0.6, lacunarity[5], p),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p),
				 0.6*octavenoise(octaves[4], 0.6, lacunarity[4], p)), lacunarity[3], p);
		}

		if (n > 0.2 && n <= 0.25) {
			n += (0.25-n)*0.2*ridged_octavenoise(octaves[3],
				clamp(h*0.0002*octavenoise(octaves[5], 0.5, lacunarity[5], p),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p),
				 0.5*octavenoise(octaves[4], 0.5, lacunarity[4], p)), lacunarity[3], p);
		} else if (n > 0.05) {
			n += ((n-0.05)/15)*ridged_octavenoise(octaves[3],
				clamp(h*0.0002*octavenoise(octaves[5], 0.5, lacunarity[5], p),
				 0.5*octavenoise(octaves[3], 0.5, lacunarity[3], p),
				 0.5*octavenoise(octaves[4], 0.5, lacunarity[4], p)), lacunarity[3], p);
		}
		//n = n*0.2;

		if (n < 0.01){
			n += n*voronoiscam_octavenoise(octaves[3],
				clamp(h*0.00002, 0.5, 0.5), lacunarity[3], p);
		} else if (n <0.02){
			n += 0.01*voronoiscam_octavenoise(octaves[3],
				clamp(h*0.00002, 0.5, 0.5), lacunarity[3], p);
		} else {
			n += (0.02/n)*0.01*voronoiscam_octavenoise(octaves[3],
				clamp(h*0.00002, 0.5, 0.5), lacunarity[3], p);
		}

		if (n < 0.001){
			n += n*3*dunes_octavenoise(octaves[2],
				1.0*octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[2], p);
		} else if (n <0.01){
			n += 0.003*dunes_octavenoise(octaves[2],
				1.0*octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[2], p);
		} else {
			n += (0.01/n)*0.003*dunes_octavenoise(octaves[2],
				1.0*octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[2], p);
		}

		if (n < 0.1){
			n += n*0.05*dunes_octavenoise(octaves[2],
				n*river_octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[2], p);
		} else if (n <0.2){
			n += 0.005*dunes_octavenoise(octaves[2],
				((n*n*10.0)+(3*(n-0.1)))*
				river_octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[2], p);
		} else {
			n += (0.2/n)*0.005*dunes_octavenoise(octaves[2],
				clamp(0.7-(1-(5*n)), 0.0, 0.7)*
				river_octavenoise(octaves[2], 0.5, lacunarity[2], p), lacunarity[2], p);
		}

		n *= 0.3;

	}

	n = maxHeight*n;
	return (n > 0.0 ? n : 0.0);
}
