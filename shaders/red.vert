// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Input vertex data, different for all executions of this shader.
in vec3 vertexPos;
in vec3 vertexNormal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

uniform float radius;

varying vec3 normal;

void main()
{
	// Normal of the the vertex, in camera space
	// Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	normal = (V * M * vec4(vertexNormal,0)).xyz;

	gl_Position =  MVP * vec4(vertexPos*radius,1.0);
} 
