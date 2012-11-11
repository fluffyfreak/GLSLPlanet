
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

	//smaller numbers give a larger feature size
	float feature_size = 1.0;
	float poo = octavenoise(3, 0.95, 2.0, p, feature_size, 1.0)/4.0;
	poo *= ridged_octavenoise(3, 0.95, 2.0, p, feature_size*2.5, 0.25)/4.0;
	poo += billow_octavenoise(3, 0.575, 2.0, p, feature_size*0.5, 1.333)/4.0;
	poo += combo_octavenoise(3, 0.7, 2.0, p, feature_size*5.675, 3.0)/4.0;

	float height = clamp(0.25+poo, 0.0, 1.0);
	gl_FragColor = vec4(height, 0.0, 0.0, 1.0);
} 
