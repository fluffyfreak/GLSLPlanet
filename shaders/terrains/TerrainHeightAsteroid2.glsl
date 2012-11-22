// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Banded/Ridged pattern mountainous terrain, could pass for desert
float GetHeight(in vec3 p)
{
	int vor_oct = 6;
	float vor_rough = 0.2*octavenoise(2, 0.3, 3.7, p, 1.0, 1.0);
	float vor_lac = 15.0*octavenoise(2, 0.5, 4.0, p, 1.0, 1.0);
	
	int ridg_oct = int(16.0*octavenoise(2, 0.275, 3.2, p, 1.0, 1.0));
	float ridg_rough = 0.4*ridged_octavenoise(4, 0.4, 3.0, p, 1.0, 1.0);
	float ridg_lac = 4.0*octavenoise(3, 0.35, 3.7, p, 1.0, 1.0);
	
	//		   voronoiscam_octavenoise(in int octaves, in float roughness, in float lacunarity, in vec3 p, in float freq, in float time) {
	float n1 = voronoiscam_octavenoise(vor_oct, vor_rough, vor_lac, p, 1.0, 1.0);
		
	//		   ridged_octavenoise(in int octaves, in float roughness, in float lacunarity, in vec3 p, in float freq, in float time)
	float n2 = ridged_octavenoise(ridg_oct, ridg_rough, ridg_lac, p, 1.0, 1.0);
		
	float n = n1 * 0.75 * n2;

	return (n > 0.0 ? maxHeight*n : 0.0);
}
