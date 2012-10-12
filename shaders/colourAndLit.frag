// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

varying vec3 lightDir, normal;
uniform sampler2D myTexture;

void main()
{

	float intensity;
	vec4 color;
	
	// normalizing the lights position to be on the safe side
	
	vec3 n = normalize(normal);
	intensity = dot(lightDir,n);
	
	color = texture2D(myTexture, vec2(gl_TexCoord[0]));
	gl_FragColor = color * intensity;
} 