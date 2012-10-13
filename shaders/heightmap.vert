// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Input vertex data, different for all executions of this shader.
in vec3 vertexPos;
in vec2 vertexUVs;

varying vec2 uv;

void main()
{
	uv = vertexUVs;
	gl_Position = vec4(vertexPos,1.0);
} 
