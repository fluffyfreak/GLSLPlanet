// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Craggy/Spikey terrain with massive canyons
float GetHeight(in vec3 p)
{
	float n = 0.07*voronoiscam_octavenoise(12, clamp(abs(0.165 - (0.38*river_octavenoise(12, 0.4, 2.5, p, 1.0, 1.0))), 0.15, 0.5),clamp(8.0*billow_octavenoise(12, 0.37, 4.0, p, 1.0, 1.0), 0.5, 9.0), p, 1.0, 1.0);

	return (n > 0.0? maxHeight*n : 0.0);
}
