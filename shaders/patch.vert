// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Input vertex data, different for all executions of this shader.
in vec3 vertexPos;
in vec2 vertexUVs;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform float radius;
uniform vec3 v0;
uniform vec3 v1;
uniform vec3 v2;
uniform vec3 v3;
uniform float fracStep;

uniform sampler2D texHeightmap; 

varying vec3 normal;
varying vec2 uv;

// in patch surface coords, [0,1]
// v[0] to v[3] are the corner vertices
vec3 GetSpherePoint(const float x, const float y) {
	return normalize(v0 + x*(1.0-y)*(v1-v0) + x*y*(v2-v0) + (1.0-x)*y*(v3-v0));
}

void main()
{
	const float heightscale = 0.05;
	uv = vertexUVs;

	// get the uv offsets
	float x = vertexUVs.x;
	float y = vertexUVs.y;
	float xm1 = x-fracStep;
	float xp1 = x+fracStep;
	float ym1 = y-fracStep;
	float yp1 = y+fracStep;

	// get the heights
	float hxm1y = (texture2D(texHeightmap, vec2(xm1,y)).x * heightscale) + 1.0;
	float hxp1y = (texture2D(texHeightmap, vec2(xp1,y)).x * heightscale) + 1.0;
	float hxym1 = (texture2D(texHeightmap, vec2(x,ym1)).x * heightscale) + 1.0;
	float hxyp1 = (texture2D(texHeightmap, vec2(x,yp1)).x * heightscale) + 1.0;

	// normal
	vec3 x1 = GetSpherePoint(xm1,y) * hxm1y;
	vec3 x2 = GetSpherePoint(xp1,y) * hxm1y;
	vec3 y1 = GetSpherePoint(x,ym1) * hxm1y;
	vec3 y2 = GetSpherePoint(x,yp1) * hxm1y;

	vec3 xNormal = normalize(cross((x2-x1),(y2-y1)));

	// Normal of the the vertex, in camera space
	// Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	normal = (V * M * vec4(xNormal,0)).xyz;

	float height = (texture2D(texHeightmap, vertexUVs).x * heightscale) + 1.0; 
	vec3 p = vertexPos * height * radius;
	gl_Position = MVP * vec4(p,1.0);
} 
