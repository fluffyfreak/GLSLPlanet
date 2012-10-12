// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Input vertex data, different for all executions of this shader.
in vec3 vertexPos;
in vec2 vertexUV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform sampler2D texHeightmap;

varying vec2 uv;

void main()
{
	uv = vertexUV;
	gl_Position =  MVP * vec4(vertexPos,1.0);
} 
