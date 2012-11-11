
uniform vec3 v0;
uniform vec3 v1;
uniform vec3 v2;
uniform vec3 v3;
uniform float fracStep;

varying vec2 uv;

// in patch surface coords, [0,1]
// v[0] to v[3] are the corner vertices
vec3 GetSpherePoint(const float x, const float y) {
	return normalize(v0 + x*(1.0-y)*(v1-v0) + x*y*(v2-v0) + (1.0-x)*y*(v3-v0));
}

void main()
{
	float xfrac = (gl_FragCoord.x-0.5) * fracStep;
	float yfrac = (gl_FragCoord.y-0.5) * fracStep;
	vec3 p = GetSpherePoint(xfrac, yfrac);

	float height = GetHeight(p);

	gl_FragColor = vec4(height, 0.0, 0.0, 1.0);
} 
