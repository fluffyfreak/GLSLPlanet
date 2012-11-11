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
	float n = octavenoise(8, 0.5, 4.0, p, 1.0, 1.0) * ridged_octavenoise(8, 0.5, 4.0, p, 1.0, 1.0);

	return (n > 0.0? maxHeight*n : 0.0);
}
