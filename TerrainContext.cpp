// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <cassert>
#include "utils.h"
#include "TerrainContext.h"
#include "GLvbo.h"
#include "vcacheopt.h"
#include "shaderHelper.h"
#include "GLfbo.h"
#include "GLprimitives.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#if TEST_CASE
#define USE_CPP_HEIGHTMAP_SHADER 0
#else
#define USE_CPP_HEIGHTMAP_SHADER 0
#endif

#if USE_CPP_HEIGHTMAP_SHADER
#include "shader_heightmap.h"
#endif

int GeoPatchContext::getIndices(std::vector<unsigned short> &pl, const unsigned int edge_hi_flags,
	unsigned short *midIndices, unsigned short *loEdgeIndices[4], unsigned short *hiEdgeIndices[4]) const
{
	// calculate how many tri's there are
	int tri_count = (VBO_COUNT_MID_IDX() / 3); 
	for( int i=0; i<4; ++i ) {
		if( edge_hi_flags & (1 << i) ) {
			tri_count += (VBO_COUNT_HI_EDGE() / 3);
		} else {
			tri_count += (VBO_COUNT_LO_EDGE() / 3);
		}
	}

	// pre-allocate enough space
	pl.reserve(tri_count);

	// add all of the middle indices
	for(int i=0; i<VBO_COUNT_MID_IDX(); ++i) {
		pl.push_back(midIndices[i]);
	}
	// selectively add the HI or LO detail indices
	for (int i=0; i<4; i++) {
		if( edge_hi_flags & (1 << i) ) {
			for(int j=0; j<VBO_COUNT_HI_EDGE(); ++j) {
				pl.push_back(hiEdgeIndices[i][j]);
			}
		} else {
			for(int j=0; j<VBO_COUNT_LO_EDGE(); ++j) {
				pl.push_back(loEdgeIndices[i][j]);
			}
		}
	}

	return tri_count;
}

// constructor
GeoPatchContext::GeoPatchContext(const uint32_t edgeLen) : 
	mEdgeLen(edgeLen), mHalfEdgeLen(edgeLen>>1), 
	mQuad(false, true), mFBO(edgeLen,edgeLen)//, mVBO(nullptr)
{
	mVertexs = new glm::vec3[NUM_MESH_VERTS()];
	mNormals = new glm::vec3[NUM_MESH_VERTS()];
	mUVs	 = new GLfloat[NUM_MESH_VERTS()*2];

	unsigned short *idx;
	unsigned short *midIndices = new unsigned short[VBO_COUNT_MID_IDX()];
	unsigned short *loEdgeIndices[4] = {nullptr};
	unsigned short *hiEdgeIndices[4] = {nullptr};
	for (int i=0; i<4; i++) {
		loEdgeIndices[i] = new unsigned short[VBO_COUNT_LO_EDGE()];
		hiEdgeIndices[i] = new unsigned short[VBO_COUNT_HI_EDGE()];
	}
	/* also want vtx indices for tris not touching edge of patch */
	idx = midIndices;
	for (uint32_t x=1; x<mEdgeLen-2; x++) {
		for (uint32_t y=1; y<mEdgeLen-2; y++) {
			idx[0] = x + mEdgeLen*y;
			idx[1] = x+1 + mEdgeLen*y;
			idx[2] = x + mEdgeLen*(y+1);
			idx+=3;

			idx[0] = x+1 + mEdgeLen*y;
			idx[1] = x+1 + mEdgeLen*(y+1);
			idx[2] = x + mEdgeLen*(y+1);
			idx+=3;
		}
	}
	{
		for (uint32_t x=1; x<mEdgeLen-3; x+=2) {
			// razor teeth near edge 0
			idx[0] = x + mEdgeLen;
			idx[1] = x+1;
			idx[2] = x+1 + mEdgeLen;
			idx+=3;
			idx[0] = x+1;
			idx[1] = x+2 + mEdgeLen;
			idx[2] = x+1 + mEdgeLen;
			idx+=3;
		}
		for (uint32_t x=1; x<mEdgeLen-3; x+=2) {
			// near edge 2
			idx[0] = x + mEdgeLen*(mEdgeLen-2);
			idx[1] = x+1 + mEdgeLen*(mEdgeLen-2);
			idx[2] = x+1 + mEdgeLen*(mEdgeLen-1);
			idx+=3;
			idx[0] = x+1 + mEdgeLen*(mEdgeLen-2);
			idx[1] = x+2 + mEdgeLen*(mEdgeLen-2);
			idx[2] = x+1 + mEdgeLen*(mEdgeLen-1);
			idx+=3;
		}
		for (uint32_t y=1; y<mEdgeLen-3; y+=2) {
			// near edge 1
			idx[0] = mEdgeLen-2 + y*mEdgeLen;
			idx[1] = mEdgeLen-1 + (y+1)*mEdgeLen;
			idx[2] = mEdgeLen-2 + (y+1)*mEdgeLen;
			idx+=3;
			idx[0] = mEdgeLen-2 + (y+1)*mEdgeLen;
			idx[1] = mEdgeLen-1 + (y+1)*mEdgeLen;
			idx[2] = mEdgeLen-2 + (y+2)*mEdgeLen;
			idx+=3;
		}
		for (uint32_t y=1; y<mEdgeLen-3; y+=2) {
			// near edge 3
			idx[0] = 1 + y*mEdgeLen;
			idx[1] = 1 + (y+1)*mEdgeLen;
			idx[2] = (y+1)*mEdgeLen;
			idx+=3;
			idx[0] = 1 + (y+1)*mEdgeLen;
			idx[1] = 1 + (y+2)*mEdgeLen;
			idx[2] = (y+1)*mEdgeLen;
			idx+=3;
		}
	}
	// full detail edge triangles
	{
		idx = hiEdgeIndices[0];
		for (uint32_t x=0; x<mEdgeLen-1; x+=2) {
			idx[0] = x; idx[1] = x+1; idx[2] = x+1 + mEdgeLen;
			idx+=3;
			idx[0] = x+1; idx[1] = x+2; idx[2] = x+1 + mEdgeLen;
			idx+=3;
		}
		idx = hiEdgeIndices[1];
		for (uint32_t y=0; y<mEdgeLen-1; y+=2) {
			idx[0] = mEdgeLen-1 + y*mEdgeLen;
			idx[1] = mEdgeLen-1 + (y+1)*mEdgeLen;
			idx[2] = mEdgeLen-2 + (y+1)*mEdgeLen;
			idx+=3;
			idx[0] = mEdgeLen-1 + (y+1)*mEdgeLen;
			idx[1] = mEdgeLen-1 + (y+2)*mEdgeLen;
			idx[2] = mEdgeLen-2 + (y+1)*mEdgeLen;
			idx+=3;
		}
		idx = hiEdgeIndices[2];
		for (uint32_t x=0; x<mEdgeLen-1; x+=2) {
			idx[0] = x + (mEdgeLen-1)*mEdgeLen;
			idx[1] = x+1 + (mEdgeLen-2)*mEdgeLen;
			idx[2] = x+1 + (mEdgeLen-1)*mEdgeLen;
			idx+=3;
			idx[0] = x+1 + (mEdgeLen-2)*mEdgeLen;
			idx[1] = x+2 + (mEdgeLen-1)*mEdgeLen;
			idx[2] = x+1 + (mEdgeLen-1)*mEdgeLen;
			idx+=3;
		}
		idx = hiEdgeIndices[3];
		for (uint32_t y=0; y<mEdgeLen-1; y+=2) {
			idx[0] = y*mEdgeLen;
			idx[1] = 1 + (y+1)*mEdgeLen;
			idx[2] = (y+1)*mEdgeLen;
			idx+=3;
			idx[0] = (y+1)*mEdgeLen;
			idx[1] = 1 + (y+1)*mEdgeLen;
			idx[2] = (y+2)*mEdgeLen;
			idx+=3;
		}
	}
	// these edge indices are for patches with no
	// neighbour of equal or greater detail -- they reduce
	// their edge complexity by 1 division
	{
		idx = loEdgeIndices[0];
		for (uint32_t x=0; x<mEdgeLen-2; x+=2) {
			idx[0] = x;
			idx[1] = x+2;
			idx[2] = x+1+mEdgeLen;
			idx += 3;
		}
		idx = loEdgeIndices[1];
		for (uint32_t y=0; y<mEdgeLen-2; y+=2) {
			idx[0] = (mEdgeLen-1) + y*mEdgeLen;
			idx[1] = (mEdgeLen-1) + (y+2)*mEdgeLen;
			idx[2] = (mEdgeLen-2) + (y+1)*mEdgeLen;
			idx += 3;
		}
		idx = loEdgeIndices[2];
		for (uint32_t x=0; x<mEdgeLen-2; x+=2) {
			idx[0] = x+mEdgeLen*(mEdgeLen-1);
			idx[2] = x+2+mEdgeLen*(mEdgeLen-1);
			idx[1] = x+1+mEdgeLen*(mEdgeLen-2);
			idx += 3;
		}
		idx = loEdgeIndices[3];
		for (uint32_t y=0; y<mEdgeLen-2; y+=2) {
			idx[0] = y*mEdgeLen;
			idx[2] = (y+2)*mEdgeLen;
			idx[1] = 1 + (y+1)*mEdgeLen;
			idx += 3;
		}
	}

	// these will hold the optimised indices
	std::vector<unsigned short> pl_short[NUM_INDEX_LISTS];
	// populate the N indices lists from the arrays built during InitTerrainIndices()
	for( int i=0; i<NUM_INDEX_LISTS; ++i ) {
		const unsigned int edge_hi_flags = i;
		mElementBuffersTriangleCounts[i] = getIndices(pl_short[i], edge_hi_flags, midIndices, loEdgeIndices, hiEdgeIndices);
	}

	// iterate over each index list and optimize it
	for( int i=0; i<NUM_INDEX_LISTS; ++i ) {
		const int tri_count = mElementBuffersTriangleCounts[i];
		VertexCacheOptimizerUShort vco;
		VertexCacheOptimizerUShort::Result res = vco.Optimize(&pl_short[i][0], tri_count);
		assert(0 == res);
	}

	// everything should be hunky-dory for setting up as OpenGL index buffers now.
	for( int i=0; i<NUM_INDEX_LISTS; ++i ) {
		glGenBuffersARB(1, &mElementBuffers[i]);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, mElementBuffers[i]);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*mElementBuffersTriangleCounts[i]*3, &(pl_short[i][0]), GL_STATIC_DRAW);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// delete temporary buffers
	if (midIndices) {
		delete [] midIndices;
		for (int i=0; i<4; i++) {
			delete [] loEdgeIndices[i];
			delete [] hiEdgeIndices[i];
		}
	}

	////////////////////////////////////////////////////////////////
	// load the quad terrain shader(s)
	/*static const std::string shaderFilenames[] = {
		"terrains/TerrainHeightAsteroid.glsl",
		"terrains/TerrainHeightAsteroid2.glsl",
		"terrains/TerrainHeightAsteroid3.glsl",
		"terrains/TerrainHeightAsteroid4.glsl",
		"terrains/TerrainHeightBarrenRock.glsl",
		"terrains/TerrainHeightBarrenRock2.glsl",
		"terrains/TerrainHeightBarrenRock3.glsl",
		"terrains/TerrainHeightFlat.glsl",
		"terrains/TerrainHeightHillsCraters.glsl",
		"terrains/TerrainHeightHillsCraters2.glsl",
		"terrains/TerrainHeightHillsDunes.glsl",
		"terrains/TerrainHeightHillsNormal.glsl",
		"terrains/TerrainHeightHillsRidged.glsl",
		"terrains/TerrainHeightHillsRivers.glsl",
		"terrains/TerrainHeightMapped.glsl",
		"terrains/TerrainHeightMapped2.glsl",
		"terrains/TerrainHeightMountainsCraters.glsl",
		"terrains/TerrainHeightMountainsCraters2.glsl",
		"terrains/TerrainHeightMountainsNormal.glsl",
		"terrains/TerrainHeightMountainsRidged.glsl",
		"terrains/TerrainHeightMountainsRivers.glsl",
		"terrains/TerrainHeightMountainsRiversVolcano.glsl",
		"terrains/TerrainHeightMountainsVolcano.glsl",
		"terrains/TerrainHeightRuggedDesert.glsl",
		"terrains/TerrainHeightRuggedLava.glsl",
		"terrains/TerrainHeightShaderFun.glsl",
		"terrains/TerrainHeightWaterSolid.glsl",
		"terrains/TerrainHeightWaterSolidCanyons.glsl",
		""
	};*/
	static const std::string shaderFilenames[] = {
		"terrains/TerrainHeightAsteroid.glsl",
		"terrains/TerrainHeightAsteroid2.glsl",
		"terrains/TerrainHeightAsteroid3.glsl",
		"terrains/TerrainHeightAsteroid4.glsl",
		"terrains/TerrainHeightBarrenRock.glsl",
		"terrains/TerrainHeightBarrenRock2.glsl",
		"terrains/TerrainHeightBarrenRock3.glsl",
		"terrains/TerrainHeightFlat.glsl",
		"terrains/TerrainHeightHillsCraters.glsl",
		"terrains/TerrainHeightHillsCraters2.glsl",
		"terrains/TerrainHeightHillsDunes.glsl",
		"terrains/TerrainHeightHillsNormal.glsl",
		"terrains/TerrainHeightHillsRidged.glsl",
		"terrains/TerrainHeightHillsRivers.glsl",
		//"terrains/TerrainHeightMapped.glsl",		// wtf to do with this?
		//"terrains/TerrainHeightMapped2.glsl",		// wtf to do with this?
		"terrains/TerrainHeightMountainsCraters.glsl",
		"terrains/TerrainHeightMountainsCraters2.glsl",
		"terrains/TerrainHeightMountainsNormal.glsl",
		"terrains/TerrainHeightMountainsRidged.glsl",
		"terrains/TerrainHeightMountainsRivers.glsl",
		//"terrains/TerrainHeightMountainsRiversVolcano.glsl",	// requires m_seed and bit shifting
		//"terrains/TerrainHeightMountainsVolcano.glsl",		// requires m_seed and bit shifting
		"terrains/TerrainHeightRuggedDesert.glsl",
		"terrains/TerrainHeightRuggedLava.glsl",
		"terrains/TerrainHeightShaderFun.glsl",
		"terrains/TerrainHeightWaterSolid.glsl",
		"terrains/TerrainHeightWaterSolidCanyons.glsl",
		""
	};
	vecBindings noiseyBinding;
	noiseyBinding.push_back( ShaderBindPair("noise_lib.glsl",eFragShader) );
	noiseyBinding.push_back( ShaderBindPair("noise_feature_lib.glsl",eFragShader) );
	int shdFnameIdx=0;
	mCurrentHeightmapProg = 0;//mHeightmapProgs.size()-1;
	while(shaderFilenames[shdFnameIdx][0] != '\0')
	{
		noiseyBinding.push_back( ShaderBindPair(shaderFilenames[shdFnameIdx],eFragShader) );
		SHeightmapGen hProg;
		const bool success = LoadShader(hProg.prog, "heightmap_gen.vert", "heightmap_gen.frag", noiseyBinding);
		if(success) {
			mCurrentHeightmapProg = shdFnameIdx;
		}
		hProg.v0			= glGetUniformLocation(hProg.prog, "v0");
		hProg.v1			= glGetUniformLocation(hProg.prog, "v1");
		hProg.v2			= glGetUniformLocation(hProg.prog, "v2");
		hProg.v3			= glGetUniformLocation(hProg.prog, "v3");
		hProg.fracStep		= glGetUniformLocation(hProg.prog, "fracStep");

		hProg.maxHeight		= glGetUniformLocation(hProg.prog, "maxHeight");
		hProg.seaLevel		= glGetUniformLocation(hProg.prog, "seaLevel");
		hProg.fracnum		= glGetUniformLocation(hProg.prog, "fracnum");

		hProg.octaves		= glGetUniformLocation(hProg.prog, "octaves");
		hProg.amplitude		= glGetUniformLocation(hProg.prog, "amplitude");
		hProg.lacunarity	= glGetUniformLocation(hProg.prog, "lacunarity");
		hProg.frequency		= glGetUniformLocation(hProg.prog, "frequency");

		checkGLError();
		mHeightmapProgs.push_back(hProg);
		noiseyBinding.pop_back();
		shdFnameIdx++;
	}
	

	////////////////////////////////////////////////////////////////
	// load the patch terrain shader
	LoadShader(patch_prog, "patch.vert", "patch.frag");
	// Get a handle for our "MVP" uniform(s)
	patch_MatrixID		= glGetUniformLocation(patch_prog, "MVP");
	patch_ViewMatrixID	= glGetUniformLocation(patch_prog, "V");
	patch_ModelMatrixID	= glGetUniformLocation(patch_prog, "M");
	patch_radius		= glGetUniformLocation(patch_prog, "radius");
	patch_v0			= glGetUniformLocation(patch_prog, "v0");
	patch_v1			= glGetUniformLocation(patch_prog, "v1");
	patch_v2			= glGetUniformLocation(patch_prog, "v2");
	patch_v3			= glGetUniformLocation(patch_prog, "v3");
	patch_fracStep		= glGetUniformLocation(patch_prog, "fracStep");
	patch_colour		= glGetUniformLocation(patch_prog, "in_colour");
	patch_texHeightmap	= glGetUniformLocation(patch_prog, "texHeightmap");

	////////////////////////////////////////////////////////////////
	// create a dummy set of verts, the UVs are the only important part
	//glm::vec3 *vts = vertexs();
	//GLfloat *pUV = uvs();
	//assert(nullptr!=vts);
	//float xfrac = 0.0f;
	//float yfrac = 0.0f;
	//for (uint32_t y=0; y<mEdgeLen; y++) {
	//	xfrac = 0.0f;
	//	for (uint32_t x=0; x<mEdgeLen; x++) {
	//		*(vts++) = glm::vec3(1.0f,1.0f,1.0f);
	//		*(pUV++) = xfrac;
	//		*(pUV++) = yfrac;
	//		xfrac += reciprocalEdgeLen();
	//	}
	//	yfrac += reciprocalEdgeLen();
	//}
	//assert(vts == &vertexs()[NUM_MESH_VERTS()]);

	//// now create the VBO
	//assert(nullptr==mVBO);
	//mVBO = new CGLvbo( NUM_MESH_VERTS(), &vertexs()[0], nullptr, uvs() );
}

// destructor
GeoPatchContext::~GeoPatchContext() {
	delete [] mVertexs;
	delete [] mNormals;
	delete [] mUVs;
}

// render the heightmap to a framebuffer
void GeoPatchContext::renderHeightmap(
	const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3, 
	const uint32_t targetTex) const
{
#if USE_CPP_HEIGHTMAP_SHADER
	const float fracStep = textureLerpStep();
	NCppHeightmapShader::setUniforms(v0, v1, v2, v3, fracStep);

	for (uint32_t y=0; y<mEdgeLen; y++) {
		for (uint32_t x=0; x<mEdgeLen; x++) {
			const float xfrac = float(x) * fracStep;
			const float yfrac = float(y) * fracStep;
			const float blah = NCppHeightmapShader::shader_heightmap_frag(vec2(float(x)+0.5f, float(y)+0.5f));
			mpHeightmapData[x + (y * mEdgeLen)] = blah;
		}
	}
#else
	// bind the framebuffer
	ScopedBindRelease<CGLfbo> fbo(mFBO);

	// setup to render to the fbo
	glViewport(0, 0, mFBO.Width(), mFBO.Height());
	checkGLError();

	// Clear the fbo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	checkGLError();

	const SHeightmapGen& rHGD = GetHeightmapGenData();
		
	// Use our fbo shader
	glUseProgram(rHGD.prog);
	checkGLError();

	glUniform3fv(rHGD.v0,		1, &v0[0]);
	glUniform3fv(rHGD.v1,		1, &v1[0]);
	glUniform3fv(rHGD.v2,		1, &v2[0]);
	glUniform3fv(rHGD.v3,		1, &v3[0]);
	const float reciprocalFBOWidth = textureLerpStep();
	glUniform1f(rHGD.fracStep, reciprocalFBOWidth);

	glUniform1f(rHGD.maxHeight,	2.0f);
	glUniform1f(rHGD.seaLevel,	0.01f);
	glUniform1i(rHGD.fracnum,	3);
	
	int octaves[10] = {3};
	float amplitude[10] = {3.0f};
	float lacunarity[10] = {3.0f};
	float frequency[10] = {3.0f};
	glUniform1iv(rHGD.octaves,		10,		&octaves[0]);
	glUniform1fv(rHGD.amplitude,	10,		&amplitude[0]);
	glUniform1fv(rHGD.lacunarity,	10,		&lacunarity[0]);
	glUniform1fv(rHGD.frequency,	10,		&frequency[0]);

	// rendering our quad now should fill the render texture with the heightmap shaders output
	renderQuad();

	mFBO.CopyTexture(targetTex);

	// the framebuffer is automatically released
#endif
}

void GeoPatchContext::renderQuad() const
{
	mQuad.Render();
	checkGLError();
}

void GeoPatchContext::UsePatchShader(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP) const {
	assert(patch_prog!=UINT_MAX);
	glUseProgram(patch_prog);
	checkGLError();

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(patch_MatrixID,		1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(patch_ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(patch_ViewMatrixID,	1, GL_FALSE, &ViewMatrix[0][0]);

	const float fracStep = textureLerpStep();
	glUniform1f(patch_fracStep, fracStep);
	glUniform1f(patch_radius, 25.0f);
	checkGLError();
}
