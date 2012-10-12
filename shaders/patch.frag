// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

uniform vec4 in_colour;

//both the sampler2D and the lookup are necessary
//even though they not actually used!
uniform sampler2D texHeightmap;
//vec4 notUsed = texture2D(texHeightmap, vec2(0.0, 0.0));

varying vec3 normal;
varying vec2 uv;

void main()
{
	vec3 lightDir = normalize(vec3(1.0,0.0,1.0));
	vec3 n = normalize(normal);

#if 0
	float intensity = dot(lightDir,n) * texture2D(texHeightmap, uv).r;
	gl_FragColor = in_colour * (1.0+intensity)*0.5;
#else
	float intensity = texture2D(texHeightmap, uv).r;
	gl_FragColor = in_colour * intensity;
#endif
} 