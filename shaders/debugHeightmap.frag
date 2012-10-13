// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform sampler2D texHeightmap;
uniform vec4 in_colour;

varying vec2 uv;

void main()
{
	float intensity = texture2D(texHeightmap, uv).x;
	gl_FragColor = in_colour * intensity;
	//vec4 xColour = vec4(uv.x,0.0,0.0,1.0);
	//vec4 yColour = vec4(0.0,0.0,uv.y,1.0);
	//gl_FragColor = xColour + yColour;
} 