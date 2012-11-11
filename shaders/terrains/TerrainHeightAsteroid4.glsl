// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Cool terrain for asteroids or small planets
float GetHeight(in vec3 p)
{
	float n = octavenoise(6, 0.2*octavenoise(2, 0.3, 3.7, p, 1.0, 1.0), 
		2.8*ridged_octavenoise(3, 0.5, 3.0, p, 1.0, 1.0), p, 1.0, 1.0) * 0.75*ridged_octavenoise(16*octavenoise(3, 0.275, 2.9, p, 1.0, 1.0), 
		0.3*octavenoise(2, 0.4, 3.0, p, 1.0, 1.0), 
		2.8*ridged_octavenoise(8, 0.35, 2.7, p, 1.0, 1.0), p, 1.0, 1.0);

	return (n > 0.0? maxHeight*n : 0.0);
}
