// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <cassert>
#include "utils.h"
#include "TerrainMesh.h"
#include "GLvbo.h"
#include "vcacheopt.h"
#include "shaderHelper.h"
#include "GLfbo.h"
#include "GLprimitives.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#define GEOPATCH_SUBDIVIDE_AT_CAMDIST	2.0f	//1.5f
#if 0
#define GEOPATCH_MAX_DEPTH  10
#else
#define GEOPATCH_MAX_DEPTH  1
#endif

class GeoPatchContext
{
private:
	////////////////////////////////////////////////////////////////
	// class static private members
	static float *s_pHeightmapData_;

	////////////////////////////////////////////////////////////////
	// private members
	const uint32_t mEdgeLen;
	const uint32_t mHalfEdgeLen;

	CGLquad	mQuad;
	//CGLvbo *mVBO;
	
	glm::vec3 * mVertexs;
	glm::vec3 * mNormals;
	GLfloat * mUVs;

	static const uint32_t NUM_INDEX_LISTS = 16;
	GLuint mElementBuffers[NUM_INDEX_LISTS];
	GLuint mElementBuffersTriangleCounts[NUM_INDEX_LISTS];

	inline int VBO_COUNT_LO_EDGE() const { return 3*(mHalfEdgeLen); }
	inline int VBO_COUNT_HI_EDGE() const { return 3*(mEdgeLen-1); }
	inline int VBO_COUNT_MID_IDX() const { return (4*3*(mEdgeLen-3)) + 2*(mEdgeLen-3)*(mEdgeLen-3)*3; }

public:
	////////////////////////////////////////////////////////////////
	// public members
	CGLfbo	mFBO;
	
	////////////////////////////////////////////////////////////////
	// public methods

	inline uint32_t NUM_MESH_VERTS() const { return mEdgeLen*mEdgeLen; }
	inline uint32_t NUM_INDICES() const { return NUM_MESH_VERTS()*2*3; }

	int getIndices(std::vector<unsigned short> &pl, const unsigned int edge_hi_flags,
		unsigned short *midIndices, unsigned short *loEdgeIndices[4], unsigned short *hiEdgeIndices[4])
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
	GeoPatchContext(const uint32_t edgeLen) : 
		mEdgeLen(edgeLen), mHalfEdgeLen(edgeLen>>1), 
		mFBO((edgeLen-1),(edgeLen-1)), mQuad(false, false)//, mVBO(nullptr)
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

		// temporary buffer where we'll retrieve the generated textures data into
		createHeightmapData();

		////////////////////////////////////////////////////////////////
		// load the quad terrain shader
		LoadShader(quad_heightmap_prog, "heightmap", "heightmap");
		quad_heightmap_v0		= glGetUniformLocation(quad_heightmap_prog, "v0");
		quad_heightmap_v1		= glGetUniformLocation(quad_heightmap_prog, "v1");
		quad_heightmap_v2		= glGetUniformLocation(quad_heightmap_prog, "v2");
		quad_heightmap_v3		= glGetUniformLocation(quad_heightmap_prog, "v3");
		quad_heightmap_fracStep	= glGetUniformLocation(quad_heightmap_prog, "fracStep");
		checkGLError();

		////////////////////////////////////////////////////////////////
		// load the patch terrain shader
		LoadShader(patch_prog, "patch", "patch");
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
	~GeoPatchContext() {
		delete [] mVertexs;
		delete [] mNormals;
		delete [] mUVs;
	}

	glm::vec3 * vertexs()	const { return mVertexs; }
	glm::vec3 * normals()	const { return mNormals; }
	GLfloat * uvs()		const { return mUVs; }

	inline GLuint elementBuffers(const GLuint iBufIndex) const { 
		assert(iBufIndex>=0 && iBufIndex<NUM_INDEX_LISTS); 
		return mElementBuffers[iBufIndex]; 
	}
	inline GLuint elementBuffersIndexCount(const GLuint iBufIndex) const { 
		assert(iBufIndex>=0 && iBufIndex<NUM_INDEX_LISTS); 
		return mElementBuffersTriangleCounts[iBufIndex]*3; 
	}

	inline uint32_t edgeLen() const { return mEdgeLen; }
	inline uint32_t halfEdgeLen() const { return mHalfEdgeLen; }
	

private:
	GLuint quad_heightmap_prog;// = 0;
	GLuint quad_heightmap_v0;
	GLuint quad_heightmap_v1;
	GLuint quad_heightmap_v2;
	GLuint quad_heightmap_v3;
	GLuint quad_heightmap_fracStep;
public:
	void renderQuad() const
	{
		mQuad.Render();
		checkGLError();
	}

	GLuint quadHeightmapProg()					const { return quad_heightmap_prog; }
	GLuint quadHeightmapV0ID()					const { return quad_heightmap_v0; }
	GLuint quadHeightmapV1ID()					const { return quad_heightmap_v1; }
	GLuint quadHeightmapV2ID()					const { return quad_heightmap_v2; }
	GLuint quadHeightmapV3ID()					const { return quad_heightmap_v3; }
	GLuint quadHeightmapFracStepID()			const { return quad_heightmap_fracStep; }

private:
	void createHeightmapData() {
		if(s_pHeightmapData_) {
			delete [] s_pHeightmapData_;
			s_pHeightmapData_ = nullptr;
		}
		s_pHeightmapData_ = new float[(int)((edgeLen()-1) * (edgeLen()-1))];
		memset(s_pHeightmapData_,0,sizeof(float)*(int)((edgeLen()-1) * (edgeLen()-1)));
	}
public:
	float *getHeightmapData() const {
		assert(s_pHeightmapData_);
		mFBO.GetData(s_pHeightmapData_);
		return s_pHeightmapData_;
	}

private:
	GLuint patch_prog;
	GLuint patch_MatrixID;
	GLuint patch_ViewMatrixID;
	GLuint patch_ModelMatrixID;
	GLuint patch_radius;
	GLuint patch_v0;
	GLuint patch_v1;
	GLuint patch_v2;
	GLuint patch_v3;
	GLuint patch_fracStep;
	GLuint patch_colour;
	GLuint patch_texHeightmap;
public:
	GLuint patchV0ID()				const { return patch_v0; }
	GLuint patchV1ID()				const { return patch_v1; }
	GLuint patchV2ID()				const { return patch_v2; }
	GLuint patchV3ID()				const { return patch_v3; }
	GLuint patchColourID()			const { return patch_colour; }
	GLuint patchTexHeightmapID()	const { return patch_texHeightmap; }

	void UsePatchShader(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP) const {
		assert(patch_prog!=UINT_MAX);
		glUseProgram(patch_prog);
		checkGLError();

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(patch_MatrixID,		1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(patch_ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(patch_ViewMatrixID,	1, GL_FALSE, &ViewMatrix[0][0]);

		const float fracStep = 1.0f/float(edgeLen()-1);
		glUniform1f(patch_fracStep, fracStep);
		glUniform1f(patch_radius, 25.0f);
		checkGLError();
	}
};
float *GeoPatchContext::s_pHeightmapData_ = nullptr;

class GeoPatch
{
private:
	////////////////////////////////////////////////////////////////
	// private members

	const GeoPatchContext &mContext;
	const GeoSphere *mpGeoSphere;
	CGLvbo *mVBO;
	GLuint mHeightmap;
	const glm::vec3 mV0, mV1, mV2, mV3;	// corner vertices for the patch
	const glm::vec3 mClipCentroid;
	glm::vec3 mCentroid;
	const uint32_t mDepth;
	float mClipRadius;
	float mRoughLength;

	typedef uint64_t TGeoPatchID;
	const uint64_t mPatchID64;

	inline TGeoPatchID calculateNextPatchID(const int depth, const int idx) const
	{
		assert(idx>=0 && idx<4);
		assert(depth<=GEOPATCH_MAX_DEPTH);
		const uint64_t idx64 = idx;
		const uint64_t shiftDepth64 = depth*2ULL;
		assert((mPatchID64 & (3i64<<shiftDepth64))==0);
		return TGeoPatchID( mPatchID64 | (idx64<<shiftDepth64) );
	}

	inline int getPatchIdx(const int depth) const
	{
		assert(depth<=GEOPATCH_MAX_DEPTH);
		const uint64_t shiftDepth64 = depth*2ULL;
		const uint64_t idx64 = (mPatchID64 & (3i64<<shiftDepth64)) >> shiftDepth64;
		assert(idx64<=UINT_MAX);
		return int(idx64);
	}

	inline int getPatchFaceIdx() const
	{
		const uint64_t maxShiftDepth = (GEOPATCH_MAX_DEPTH+1)*2;
		const int res = (mPatchID64 & (7i64 << maxShiftDepth)) >> maxShiftDepth;
		assert(res>=0 && res<6);
		return res;
	}
	

public:
	////////////////////////////////////////////////////////////////
	// public members

	static const uint32_t NUM_EDGES = 4;
	GeoPatch *edgeFriend[NUM_EDGES];

	static const uint32_t NUM_KIDS = NUM_EDGES;
	GeoPatch *kids[NUM_KIDS];

	GeoPatch *parent;

	////////////////////////////////////////////////////////////////
	// public methods

	// constructor
	GeoPatch(const GeoPatchContext &context_, const GeoSphere *pGeoSphere_, 
		const glm::vec3 &v0_, const glm::vec3 &v1_, const glm::vec3 &v2_, const glm::vec3 &v3_, 
		const uint32_t depth_, const TGeoPatchID ID_)
		: mContext(context_), mpGeoSphere(pGeoSphere_), mHeightmap(0), mVBO(nullptr), mV0(v0_), mV1(v1_), mV2(v2_), mV3(v3_), 
		mClipCentroid((v0_+v1_+v2_+v3_) * 0.25f), mDepth(depth_), mClipRadius(0.0f), mRoughLength(0.0f), 
		mPatchID64(ID_), parent(nullptr)
	{
		for (int i=0; i<NUM_KIDS; i++) {
			edgeFriend[i]	= nullptr;
			kids[i]			= nullptr;
		}

		mClipRadius = std::max(mClipRadius, glm::length(mV0-mClipCentroid));
		mClipRadius = std::max(mClipRadius, glm::length(mV1-mClipCentroid));
		mClipRadius = std::max(mClipRadius, glm::length(mV2-mClipCentroid));
		mClipRadius = std::max(mClipRadius, glm::length(mV3-mClipCentroid));

		const float fDepth = float(Clamp<uint32_t>(mDepth, 1, 5));
 		const float distMult = 5.0f / fDepth;
		mRoughLength = GEOPATCH_SUBDIVIDE_AT_CAMDIST / powf(2.0f, fDepth) * distMult;
	}

	// destructor
	~GeoPatch() 
	{
		for (int i=0; i<4; i++) {
			if (edgeFriend[i]) {
				edgeFriend[i]->NotifyEdgeFriendDeleted(this);
			}
		}
		for (int i=0; i<NUM_KIDS; i++) {
			edgeFriend[i] = nullptr;
			if(kids[i]) {
				delete kids[i];
				kids[i] = nullptr;
			}
		}

		if(glIsTexture(mHeightmap)==GL_TRUE) {
			glDeleteTextures(1, &mHeightmap);
			checkGLError();
		}
		if( mVBO ) {
			delete mVBO;
			mVBO = nullptr;
		}
	}

	// in patch surface coords, [0,1]
	glm::vec3 GetSpherePoint(const float x, const float y) {
		return glm::normalize((mV0 + x*(1.0f-y)*(mV1-mV0) + x*y*(mV2-mV0) + (1.0f-x)*y*(mV3-mV0)));
	}

	// Generates full-detail vertices, and also non-edge normals and colors
	void GenerateMesh() {
		mCentroid = glm::normalize(mClipCentroid);

		// render the heightmap to a framebuffer
		{
			// bind the framebuffer
			ScopedBindRelease<CGLfbo> fbo(mContext.mFBO);

			// setup to render to the fbo
			glViewport(0, 0, mContext.mFBO.Width(), mContext.mFBO.Height());
			checkGLError();

			// Clear the fbo
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			checkGLError();
		
			// Use our fbo shader
			glUseProgram(mContext.quadHeightmapProg());
			checkGLError();

			glUniform3fv(mContext.quadHeightmapV0ID(),		1, &mV0[0]);
			glUniform3fv(mContext.quadHeightmapV1ID(),		1, &mV1[0]);
			glUniform3fv(mContext.quadHeightmapV2ID(),		1, &mV2[0]);
			glUniform3fv(mContext.quadHeightmapV3ID(),		1, &mV3[0]);
			const float reciprocalFBOWidth = 1.0f/float(mContext.mFBO.Width()-1);
			glUniform1f(mContext.quadHeightmapFracStepID(), reciprocalFBOWidth);
			checkGLError();

			// rendering our quad now should fill the render texture with the heightmap shaders output
			mContext.renderQuad();

			// the framebuffer is automatically released
		}

		//download generated heightmap
		const float* heightmap_ = mContext.getHeightmapData();

		// Now we need to create the texture which will contain the heightmap. 
		glGenTextures(1, &mHeightmap);
		checkGLError();
 
		// Bind the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, mHeightmap);
		checkGLError();
 
		// Give the heightmap values to OpenGL ( the last parameter )
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mContext.mFBO.Width(), mContext.mFBO.Height(), 0, GL_LUMINANCE, GL_FLOAT, heightmap_);
		checkGLError();
		
#if 0
		// Good filtering needed
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
		// Bad filtering needed
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		checkGLError();

		// release the texture binding
		glBindTexture(GL_TEXTURE_2D, 0);

		////////////////////////////////////////////////////////////////
		// create a dummy set of verts, the UVs are the only important part
		glm::vec3 *vts = mContext.vertexs();
		GLfloat *pUV = mContext.uvs();
		assert(nullptr!=vts);
		const float fracStep = 1.0f/float(mContext.edgeLen()-1);
		float xfrac = 0.0f;
		float yfrac = 0.0f;
		for (uint32_t y=0; y<mContext.edgeLen(); y++) {
			xfrac = 0.0f;
			for (uint32_t x=0; x<mContext.edgeLen(); x++) {
				assert(xfrac<=1.0f && yfrac<=1.0f);
				const glm::vec3 p = GetSpherePoint(xfrac, yfrac);
				*(vts++) = p;

				//static glm::vec2 s_uv[4] = {
				//	glm::vec2(0.0f, 0.0f),
				//	glm::vec2(1.0f, 0.0f),
				//	glm::vec2(1.0f, 1.0f),
				//	glm::vec2(0.0f, 1.0f)
				//};
				//// lerp ( a + t * (b - a) )
				//const glm::vec2 uvA = s_uv[0]+xfrac*(s_uv[1]-s_uv[0]);
				//const glm::vec2 uvB = s_uv[3]+xfrac*(s_uv[2]-s_uv[3]);
				//const glm::vec2 uvC = uvA+yfrac*(uvB-uvA);
				//*(pUV++) = uvC.s;
				//*(pUV++) = uvC.t;

				*(pUV++) = xfrac;
				*(pUV++) = yfrac;

				// next increment
				xfrac += fracStep;
			}
			// next increment
			yfrac += fracStep;
		}
		assert(vts == &mContext.vertexs()[mContext.NUM_MESH_VERTS()]);
		assert(pUV == &mContext.uvs()[mContext.NUM_MESH_VERTS()*2]);

		// now create the VBO
		assert(nullptr==mVBO);
		mVBO = new CGLvbo( mContext.NUM_MESH_VERTS(), &mContext.vertexs()[0], nullptr, mContext.uvs() );
	}

	inline void OnEdgeFriendChanged(const int edge, GeoPatch *e) {
		edgeFriend[edge] = e;
	}
	void NotifyEdgeFriendSplit(GeoPatch *e) {
		if (!kids[0]) {
			return;
		}
		const int idx = GetEdgeIdxOf(e);
		const int we_are = e->GetEdgeIdxOf(this);
		// match e's new kids to our own... :/
		kids[idx]->OnEdgeFriendChanged(idx, e->kids[(we_are+1)%4]);
		kids[(idx+1)%4]->OnEdgeFriendChanged(idx, e->kids[we_are]);
	}

	void NotifyEdgeFriendDeleted(const GeoPatch *e) {
		const int idx = GetEdgeIdxOf(e);
		assert(idx>=0 && idx<4);
		edgeFriend[idx] = nullptr;
	}

	inline int GetEdgeIdxOf(const GeoPatch *e) const {
		for (int i=0; i<4; i++) {
			if (edgeFriend[i] == e) {
				return i;
			}
		}
		assert(false);
		return -1;
	}

	GeoPatch *GetEdgeFriendForKid(const int kid, const int edge) const {
		const GeoPatch *e = edgeFriend[edge];
		if (!e) {
			return nullptr;
		}
		const int we_are = e->GetEdgeIdxOf(this);
		assert(we_are>=0 && we_are<4);
		// neighbour patch has not split yet (is at depth of this patch), so kids of this patch do
		// not have same detail level neighbours yet
		if (edge == kid) {
			return e->kids[(we_are+1)%4];
		} 
		return e->kids[we_are];
	}

	GLuint determineIndexbuffer() const {
		return // index buffers are ordered by edge resolution flags
			(edgeFriend[0] ? 1u : 0u) |
			(edgeFriend[1] ? 2u : 0u) |
			(edgeFriend[2] ? 4u : 0u) |
			(edgeFriend[3] ? 8u : 0u);
	}

	void Render()
	{
#if 0
		if (kids[0]) {
			for (int i=0; i<4; i++) {
				kids[i]->Render();
			}
		} 
		else 
#endif
		{
#if 1
			//glm::vec4 patchColour(float(mDepth+1) * (1.0f/float(GEOPATCH_MAX_DEPTH)), 0.0f, float(GEOPATCH_MAX_DEPTH-mDepth) * (1.0f/float(GEOPATCH_MAX_DEPTH)), 1.0f);
			static const glm::vec4 faceColours[6] = {
				glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),	// problem child - red (meets purple & cyan)
				glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),	// green
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),	// blue
				glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),	// purple
				glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),	// cyan
				glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)	// problem child - yellow (meets purple & blue)
			};
			glUniform4fv(mContext.patchColourID(), 1, &faceColours[getPatchFaceIdx()][0]);
#else
			glUniform4fv(mContext.patchColourID(), 1, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0]);
#endif
			glUniform3fv(mContext.patchV0ID(), 1, &mV0[0]);
			glUniform3fv(mContext.patchV1ID(), 1, &mV1[0]);
			glUniform3fv(mContext.patchV2ID(), 1, &mV2[0]);
			glUniform3fv(mContext.patchV3ID(), 1, &mV3[0]);
			glUniform1i(mContext.patchTexHeightmapID(), 0); //Texture unit 0 is for base images.

			// bind the heightmap texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mHeightmap);

			assert(nullptr != mVBO);
			ScopedBindRelease<CGLvbo> vbo(*mVBO);
			
			// Index buffer
			const GLuint iBufIndex = determineIndexbuffer();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mContext.elementBuffers(iBufIndex));

			// Draw the triangles !
			glDrawElements(
				GL_TRIANGLES,									// mode
				mContext.elementBuffersIndexCount(iBufIndex),	// count
				GL_UNSIGNED_SHORT,								// type
				nullptr											// element array buffer offset
			);
			checkGLError();
		}
	}

	void LODUpdate(const glm::vec3 &campos) {
		bool canSplit = true;
		// always split at first level
		if (parent) {
			for (int i=0; i<4; i++) {
				if (!edgeFriend[i]) { 
					canSplit = false; 
					break; 
				} else if (edgeFriend[i]->mDepth < mDepth) {
					canSplit = false;
					break;
				}
			}
			const float centroidDist = glm::length(campos - mCentroid);
			const bool errorSplit = (centroidDist < mRoughLength);
			if( !(canSplit && (mDepth < GEOPATCH_MAX_DEPTH) && errorSplit) ) 
			{
				canSplit = false;
			}
		}

		if (canSplit) {
			if (!kids[0]) {
				const glm::vec3 v01 = glm::normalize(mV0+mV1);
				const glm::vec3 v12 = glm::normalize(mV1+mV2);
				const glm::vec3 v23 = glm::normalize(mV2+mV3);
				const glm::vec3 v30 = glm::normalize(mV3+mV0);
				const glm::vec3 cn = glm::normalize(mCentroid);
				//const glm::vec3 cn = glm::normalize((v01+v12)+(v23+v30));
				const int newDepth = mDepth+1;
				kids[0] = new GeoPatch(mContext, mpGeoSphere, mV0, v01, cn, v30, newDepth, calculateNextPatchID(newDepth,0));
				kids[1] = new GeoPatch(mContext, mpGeoSphere, v01, mV1, v12, cn, newDepth, calculateNextPatchID(newDepth,1));
				kids[2] = new GeoPatch(mContext, mpGeoSphere, cn, v12, mV2, v23, newDepth, calculateNextPatchID(newDepth,2));
				kids[3] = new GeoPatch(mContext, mpGeoSphere, v30, cn, v23, mV3, newDepth, calculateNextPatchID(newDepth,3));
				// hm.. edges. Not right to pass this
				// edgeFriend...
				kids[0]->edgeFriend[0] = GetEdgeFriendForKid(0, 0);
				kids[0]->edgeFriend[1] = kids[1];
				kids[0]->edgeFriend[2] = kids[3];
				kids[0]->edgeFriend[3] = GetEdgeFriendForKid(0, 3);
				kids[1]->edgeFriend[0] = GetEdgeFriendForKid(1, 0);
				kids[1]->edgeFriend[1] = GetEdgeFriendForKid(1, 1);
				kids[1]->edgeFriend[2] = kids[2];
				kids[1]->edgeFriend[3] = kids[0];
				kids[2]->edgeFriend[0] = kids[1];
				kids[2]->edgeFriend[1] = GetEdgeFriendForKid(2, 1);
				kids[2]->edgeFriend[2] = GetEdgeFriendForKid(2, 2);
				kids[2]->edgeFriend[3] = kids[3];
				kids[3]->edgeFriend[0] = kids[0];
				kids[3]->edgeFriend[1] = kids[2];
				kids[3]->edgeFriend[2] = GetEdgeFriendForKid(3, 2);
				kids[3]->edgeFriend[3] = GetEdgeFriendForKid(3, 3);
				kids[0]->parent = kids[1]->parent = kids[2]->parent = kids[3]->parent = this;
				for (int i=0; i<4; i++) {
					kids[i]->GenerateMesh();
				}
				for (int i=0; i<4; i++) {
					edgeFriend[i]->NotifyEdgeFriendSplit(this);
				}
			} else {
				for (int i=0; i<4; i++) {
					kids[i]->LODUpdate(campos);
				}
			}
		} else {
			if (kids[0]) {
				for (int i=0; i<4; i++) { 
					delete kids[i]; 
					kids[i] = nullptr; 
				}
			}
		}
	}

#ifdef _DEBUG
	GLuint getHeightmapID() const { return mHeightmap; }
#endif
};

GeoSphere::GeoSphere() : mGeoPatchContext(nullptr)
{
	for (int i=0; i<NUM_PATCHES; i++) {
		mGeoPatches[i] = nullptr;
	}
}

GeoSphere::~GeoSphere()
{
	for (int i=0; i<NUM_PATCHES; i++) {
		delete mGeoPatches[i];
		mGeoPatches[i] = nullptr;
	}

	delete mGeoPatchContext;
	mGeoPatchContext = nullptr;
}

void GeoSphere::Update(const glm::vec3 &campos)
{
	if(nullptr==mGeoPatches[0]) {
		BuildFirstPatches();
	} else {
		for (int i=0; i<NUM_PATCHES; i++) {
			mGeoPatches[i]->LODUpdate(campos);
		}
	}
}

void GeoSphere::Render(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP)
{
	// setup the basics for the patch shader,
	// individual patches will change settings to match their own parameters
	mGeoPatchContext->UsePatchShader(ViewMatrix, ModelMatrix, MVP);

	checkGLError();
	for (int i=0; i<NUM_PATCHES; i++) {
		mGeoPatches[i]->Render();
	}
}

static const int geo_sphere_edge_friends[6][4] = {
	{ 3, 4, 1, 2 },
	{ 0, 4, 5, 2 },
	{ 0, 1, 5, 3 },
	{ 0, 2, 5, 4 },
	{ 0, 3, 5, 1 },
	{ 1, 4, 3, 2 }
};

void GeoSphere::BuildFirstPatches()
{
	assert(nullptr==mGeoPatchContext);
#if 0
	mGeoPatchContext = new GeoPatchContext(33);
#else
	mGeoPatchContext = new GeoPatchContext(3);//33);
#endif
	assert(nullptr!=mGeoPatchContext);

	// generate root face patches of the cube/sphere
	static const glm::vec3 p1 = glm::normalize(glm::vec3( 1, 1, 1));
	static const glm::vec3 p2 = glm::normalize(glm::vec3(-1, 1, 1));
	static const glm::vec3 p3 = glm::normalize(glm::vec3(-1,-1, 1));
	static const glm::vec3 p4 = glm::normalize(glm::vec3( 1,-1, 1));
	static const glm::vec3 p5 = glm::normalize(glm::vec3( 1, 1,-1));
	static const glm::vec3 p6 = glm::normalize(glm::vec3(-1, 1,-1));
	static const glm::vec3 p7 = glm::normalize(glm::vec3(-1,-1,-1));
	static const glm::vec3 p8 = glm::normalize(glm::vec3( 1,-1,-1));

	const uint64_t maxShiftDepth = (GEOPATCH_MAX_DEPTH+1)*2;
	mGeoPatches[0] = new GeoPatch(*mGeoPatchContext, this, p1, p2, p3, p4, 0, (0i64 << maxShiftDepth));
	mGeoPatches[1] = new GeoPatch(*mGeoPatchContext, this, p4, p3, p7, p8, 0, (1i64 << maxShiftDepth));
	mGeoPatches[2] = new GeoPatch(*mGeoPatchContext, this, p1, p4, p8, p5, 0, (2i64 << maxShiftDepth));
	mGeoPatches[3] = new GeoPatch(*mGeoPatchContext, this, p2, p1, p5, p6, 0, (3i64 << maxShiftDepth));
	mGeoPatches[4] = new GeoPatch(*mGeoPatchContext, this, p3, p2, p6, p7, 0, (4i64 << maxShiftDepth));
	mGeoPatches[5] = new GeoPatch(*mGeoPatchContext, this, p8, p7, p6, p5, 0, (5i64 << maxShiftDepth));
	for (int i=0; i<NUM_PATCHES; i++) {
		for (int j=0; j<4; j++) {
			mGeoPatches[i]->edgeFriend[j] = mGeoPatches[geo_sphere_edge_friends[i][j]];
		}
	}
	for (int i=0; i<NUM_PATCHES; i++) {
		mGeoPatches[i]->GenerateMesh();
	}

#ifdef _DEBUG
	SetupDebugRendering();
#endif
}

#ifdef _DEBUG
static const glm::vec4 faceColours[6] = {
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),	// problem child - red (meets purple & cyan)
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),	// green
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),	// blue
	glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),	// purple
	glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),	// cyan
	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)	// problem child - yellow (meets purple & blue)
};

void GeoSphere::RenderCube(glm::mat4 &ViewMatrix)
{
	if(mGeoPatches[0]==nullptr) {
		return;
	}

	// Use our shader
	glUseProgram(simple_shader);
	checkGLError();

	// Compute the MVP matrix from keyboard and mouse input
	glm::mat4 ProjectionMatrix = glm::perspective(90.0f, (4.0f/3.0f), 0.1f, 1000.f);
	glm::mat4 ModelMatrix	   = glm::translate(glm::mat4(1.0), glm::vec3(0.0,0.0,0.0));
	ModelMatrix = glm::scale(ModelMatrix, vec3(2.5));
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUniformMatrix4fv(simple_MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(simple_ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(simple_ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	////////////////////////////////////////////////////////////////
	// render each of the LOD0 heightmaps
	for (int i=0; i<6; i++) {
		glUniform1i(simple_texHeightmap, 0); //Texture unit 0 is for base images.
		glUniform4fv(simple_colour, 1, &faceColours[i][0]);

		mpCube->Render();
		checkGLError();
	}
	checkGLError();

	glUseProgram(0);
	checkGLError();
}

void GeoSphere::SetupDebugRendering()
{
	mpUVquad = new CGLquad(false, true);
	GLuint cubeTextures[6];
	for (int i=0; i<6; i++) {
		cubeTextures[i] = mGeoPatches[i]->getHeightmapID();
	}
//	mpCube = new CGLcube(false, true, cubeTextures, faceColours, simple_colour);

	// load the shaders
	////////////////////////////////////////////////////////////////
	simple_shader = 0;
	const std::string shader_vert_name = "debugHeightmap";
	const std::string shader_frag_name = "debugHeightmap";
	LoadShader(simple_shader, shader_vert_name, shader_frag_name);
	checkGLError();

	glUseProgram(simple_shader);

	// Get a handle for our "MVP" uniform
	simple_MatrixID			= glGetUniformLocation(simple_shader, "MVP");
	simple_ViewMatrixID		= glGetUniformLocation(simple_shader, "V");
	simple_ModelMatrixID	= glGetUniformLocation(simple_shader, "M");
	simple_texHeightmap		= glGetUniformLocation(simple_shader, "texHeightmap");
	simple_colour			= glGetUniformLocation(simple_shader, "in_colour");
	checkGLError();

	glUseProgram(0);

	mpCube = new CGLcube(false, true, cubeTextures, faceColours, simple_colour);
}

void GeoSphere::RenderLOD0Heightmaps()
{
	static const glm::vec3 offsets[NUM_PATCHES] = {
		glm::vec3( 2.1, 0.0, 0.0),
		glm::vec3(-2.1, 2.1, 0.0),
		glm::vec3(-4.2, 0.0, 0.0),
		glm::vec3(-2.1,-2.1, 0.0),
		glm::vec3( 0.0, 0.0, 0.0),
		glm::vec3(-2.1, 0.0, 0.0)
	};

	if(mGeoPatches[0]==nullptr) {
		return;
	}

	glDisable(GL_DEPTH_TEST);

	// Use our shader
	glUseProgram(simple_shader);
	checkGLError();

	// Compute the MVP matrix from keyboard and mouse input
	glm::mat4 ProjectionMatrix = glm::perspective(90.0f, (4.0f/3.0f), 0.1f, 1000.f);
	glm::mat4 ViewMatrix       = glm::lookAt(
									glm::vec3(5.f, 4.f, 8.f),	// Camera is here
									glm::vec3(5.f, 4.f, 0.f),	// and looks here : at the same position, plus "direction"
									glm::vec3(0.f, 1.f, 0.f)	// Head is up (set to 0,-1,0 to look upside-down)
								);
	glm::mat4 ModelMatrix	   = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	////////////////////////////////////////////////////////////////
	// render each of the LOD0 heightmaps
	for (int i=0; i<NUM_PATCHES; i++) {
		ModelMatrix	   = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, offsets[i]);
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(simple_MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(simple_ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(simple_ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		checkGLError();
		glUniform1i(simple_texHeightmap, 0); //Texture unit 0 is for base images.
		glUniform4fv(simple_colour, 1, &faceColours[i][0]);

		glActiveTexture(GL_TEXTURE0);
		const GLuint texID = mGeoPatches[i]->getHeightmapID();
		if(GL_TRUE==glIsTexture(texID)) {
			glBindTexture(GL_TEXTURE_2D, texID);
			checkGLError();
			mpUVquad->Render();
		}
	}
	checkGLError();

	glUseProgram(0);
	checkGLError();

	glEnable(GL_DEPTH_TEST);
}
#endif