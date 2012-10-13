// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

varying vec3 normal;

uniform vec4 in_colour;

void main()
{
	vec3 lightDir = normalize(vec3(1.0,0.0,1.0));
	vec3 n = normalize(normal);
	float intensity = dot(lightDir,n);
	gl_FragColor = in_colour * intensity;;
} 