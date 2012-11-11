
// Creates small canyons.
float canyon_ridged_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = ridged_octavenoise(octaves, 0.54, 2.0, frequency*p);
	float outer = 0.71;
	float inner = 0.715;
	float inner2 = 0.715;
	float outer2 = 0.72;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

// Larger canyon.
float canyon2_ridged_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0; //octavenoise(octaves, 0.56, 2.0, frequency*p);
	n = ridged_octavenoise(octaves, 0.56, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

// Largest and best looking canyon, combine them together for best results.
float canyon3_ridged_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0; //octavenoise(octaves, 0.585, 2.0, frequency*p);
	n = ridged_octavenoise(octaves, 0.585, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon_normal_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.54, 2.0, frequency*p);
	float outer = 0.71;
	float inner = 0.715;
	float inner2 = 0.715;
	float outer2 = 0.72;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon2_normal_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.56, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon3_normal_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.585, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon_voronoi_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.54, 2.0, frequency*p);
	float outer = 0.71;
	float inner = 0.715;
	float inner2 = 0.715;
	float outer2 = 0.72;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon2_voronoi_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.56, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon3_voronoi_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.585, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon_billow_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.54, 2.0, frequency*p);
	float outer = 0.71;
	float inner = 0.715;
	float inner2 = 0.715;
	float outer2 = 0.72;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon2_billow_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.56, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float canyon3_billow_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float h;
	float n = 0.0;
	n = octavenoise(octaves, 0.585, 2.0, frequency*p);
	float outer = 0.7;
	float inner = 0.71;
	float inner2 = 0.72;
	float outer2 = 0.73;
	if (n > outer2) {
		h = 1.0;
	} else if (n > inner2) {
		h = 0.0+1.0*(n-inner2)*(1.0/(outer2-inner2));
	} else if (n > inner) {
		h = 0.0;
	} else if (n > outer) {
		h = 1.0-1.0*(n-outer)*(1.0/(inner-outer));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}

float crater_function_1pass(in vec3 p, float inp, float height)
{
	float res = inp;
	float n = fabs(noise(p));
	float ejecta_outer = 0.6;
	float outer = 0.9;
	float inner = 0.94;
	float midrim = 0.93;
	if (n > inner) {
		//res = 0;
	} else if (n > midrim) {
		float hrim = inner - midrim;
		float descent = (hrim-(n-midrim))/hrim;
		res += height * descent * descent;
	} else if (n > outer) {
		float hrim = midrim - outer;
		float ascent = (n-outer)/hrim;
		res += height * ascent * ascent;
	} else if (n > ejecta_outer) {
		// blow down walls of other craters too near this one,
		// so we don't have sharp transition
		//res *= (outer-n)/-(ejecta_outer-outer);
	}
	return res;
}

// makes large and small craters across the entire planet.
float crater_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float crater = 0.0;
	float sz = frequency;
	float max_h = amplitude;
	for (int i=0; i<octaves; i++) {
		crater = crater_function_1pass(sz*p, crater, max_h);
		sz *= 2.0;
		max_h *= 0.5;
	}
	return crater;
}

float impact_crater_function_1pass(in vec3 p, float inp, float height)
{
	float n = fabs(noise(p));
	float ejecta_outer = 0.6;
	float outer = 0.9;
	float midrim = 0.93;
	float hrim;
	float descent;
	if (n > midrim) {
		res -= height;
	} else if (n > outer) {
		hrim = midrim - outer;
		descent = (n-outer)/hrim;
		res -= height * descent * descent;
	} else if (n > ejecta_outer) {
		// blow down walls of other craters too near this one,
		// so we don't have sharp transition
		//res *= (outer-n)/-(ejecta_outer-outer);
	}
	return res;
}

// makes large and small craters across the entire planet.
float impact_crater_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float crater = 0.0;
	float sz = frequency;
	float max_h = amplitude;
	for (int i=0; i<octaves; i++) {
		crater = impact_crater_function_1pass(sz*p, crater, max_h);
		sz *= 2.0;
		max_h *= 0.5;
	}
	return crater;
}

float volcano_function_1pass(in vec3 p, float inp, float height)
{
	float n = fabs(noise(p));
	float ejecta_outer = 0.6;
	float outer = 0.9;
	float inner = 0.975;
	float midrim = 0.971;
	if (n > inner) {
		//res = 0;
	} else if (n > midrim) {
		float hrim = inner - midrim;
		float descent = (hrim-(n-midrim))/hrim;
		res += height * descent;
	} else if (n > outer) {
		float hrim = midrim - outer;
		float ascent = (n-outer)/hrim;
		res += height * ascent * ascent;
	} else if (n > ejecta_outer) {
		// blow down walls of other craters too near this one,
		// so we don't have sharp transition
		res *= (outer-n)/-(ejecta_outer-outer);
	}
	return res;
}

float volcano_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float crater = 0.0;
	float sz = frequency;
	float max_h = amplitude;
	for (int i=0; i<octaves; i++) {
		crater = volcano_function_1pass(sz*p, crater, max_h);
		sz *= 1.0;  //frequency?
		max_h *= 0.4; // height??
	}
	return 3.0 * crater;
}

float megavolcano_function_1pass(in vec3 p, float inp, float height)
{
	float res = inp;
	float n = fabs(noise(p));
	float ejecta_outer = 0.6;
	float outer = 0.76;  //Radius
	float inner = 0.98;
	float midrim = 0.964;
	if (n > inner) {
		//res = 0;
	} else if (n > midrim) {
		float hrim = inner - midrim;
		float descent = (hrim-(n-midrim))/hrim;
		res += height * descent;
	} else if (n > outer) {
		float hrim = midrim - outer;
		float ascent = (n-outer)/hrim;
		res += height * ascent * ascent;
	} else if (n > ejecta_outer) {
		// blow down walls of other craters too near this one,
		// so we don't have sharp transition
		res *= (outer-n)/-(ejecta_outer-outer);
	}
	return res;
}

float megavolcano_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p)
{
	float crater = 0.0;
	float sz = frequency;
	float max_h = amplitude;
	for (int i=0; i<octaves; i++) {
		crater = megavolcano_function_1pass(sz*p, crater, max_h);
		sz *= 1.0;  //frequency?
		max_h *= 0.15; // height??
	}
	return 4.0 * crater;
}

float river_function(in int octaves, in float amplitude, in float frequency, in float lacunarity, in vec3 p, int style)
{
	float h;
	float n = octavenoise(octaves, 0.585, 2.0, frequency*p*0.5);
	float outer[2] = float[2](0.67, 0.01);
	float inner[2] = float[2](0.715, 0.49);
	float inner2[2] = float[2](0.715, 0.51);
	float outer2[2] = float[2](0.76, 0.99);
	if (n > outer2[style]) {
		h = 1.0;
	} else if (n > inner2[style]) {
		h = 0.0+1.0*(n-inner2[style])*(1.0/(outer2[style]-inner2[style]));
	} else if (n > inner[style]) {
		h = 0.0;
	} else if (n > outer[style]) {
		h = 1.0-1.0*(n-outer[style])*(1.0/(inner[style]-outer[style]));
	} else {
		h = 1.0;
	}
	return h * amplitude;
}
