// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

varying vec3 lightDir,normal;
uniform float time;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	lightDir = normalize(vec3(gl_LightSource[0].position));
	normal = gl_NormalMatrix * gl_Normal;

	gl_Position = vec4(gl_ModelViewProjectionMatrix * gl_Vertex);
} 
