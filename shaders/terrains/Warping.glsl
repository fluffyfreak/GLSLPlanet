// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

uniform float maxHeight;
uniform float seaLevel;
uniform int fracnum;

uniform int octaves[10];
uniform float amplitude[10];
uniform float lacunarity[10];
uniform float frequency[10];

// Example used for the animated Star surface in s20dan's ShaderFun branch
float GetHeight(in vec3 p)
{	
	float height = fbm( p + fbm( p + fbm( p ) ) ) * 0.1;
	return height;
}
