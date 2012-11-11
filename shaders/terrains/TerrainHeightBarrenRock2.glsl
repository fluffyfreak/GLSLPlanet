// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Strange world, looks like its been hit by thousands of years of erosion. 
// Could be used as a basis for terrains that should have erosion.
float GetHeight(in vec3 p)
{
	float n = billow_octavenoise(16, 0.3*octavenoise(8, 0.4, 2.5, p),Clamp(5.0*ridged_octavenoise(8, 0.377, 4.0, p), 1.0, 5.0), p);

	return (n > 0.0? maxHeight*n : 0.0);
}
