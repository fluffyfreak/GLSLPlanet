// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <cassert>
#include "utils.h"
#include "TerrainMesh.h"
#include "TerrainPatch.h"
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

GeoPatch::TGeoPatchID GeoPatch::calculateNextPatchID(const int depth, const int idx) const
{
	assert(idx>=0 && idx<4);
	assert(depth<=GEOPATCH_MAX_DEPTH);
	const uint64_t idx64 = idx;
	const uint64_t shiftDepth64 = depth*2ULL;
	assert((mPatchID64 & (3i64<<shiftDepth64))==0);
	return TGeoPatchID( mPatchID64 | (idx64<<shiftDepth64) );
}

int GeoPatch::getPatchIdx(const int depth) const
{
	assert(depth<=GEOPATCH_MAX_DEPTH);
	const uint64_t shiftDepth64 = depth*2ULL;
	const uint64_t idx64 = (mPatchID64 & (3i64<<shiftDepth64)) >> shiftDepth64;
	assert(idx64<=UINT_MAX);
	return int(idx64);
}

int GeoPatch::getPatchFaceIdx() const
{
	const uint64_t maxShiftDepth = (GEOPATCH_MAX_DEPTH+1)*2;
	const int res = (mPatchID64 & (7i64 << maxShiftDepth)) >> maxShiftDepth;
	assert(res>=0 && res<6);
	return res;
}

// constructor
GeoPatch::GeoPatch(const GeoPatchContext &context_, const GeoSphere *pGeoSphere_, 
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
GeoPatch::~GeoPatch() 
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

// Generates full-detail vertices, and also non-edge normals and colors
void GeoPatch::GenerateMesh() {
	mCentroid = glm::normalize(mClipCentroid);

	// render the heightmap to a framebuffer
	mContext.renderHeightmap(mV0, mV1, mV2, mV3);

	//download generated heightmap
	const float* heightmap_ = mContext.getHeightmapData();

	// Now we need to create the texture which will contain the heightmap. 
	glGenTextures(1, &mHeightmap);
	checkGLError();
 
	// Bind the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, mHeightmap);
	checkGLError();
 
	// Give the heightmap values to OpenGL ( the last parameter )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mContext.fboWidth(), mContext.fboWidth(), 0, GL_LUMINANCE, GL_FLOAT, heightmap_);
	checkGLError();
		
#if TEST_CASE
	// Bad filtering needed
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
	// Good filtering needed
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	checkGLError();

	// release the texture binding
	glBindTexture(GL_TEXTURE_2D, 0);

	////////////////////////////////////////////////////////////////
	// Create the base mesh that the heightmap will modify
	glm::vec3 *vts = mContext.vertexs();
	GLfloat *pUV = mContext.uvs();
	assert(nullptr!=vts);
	const float fracStep = mContext.meshLerpStep();
	for (uint32_t y=0; y<mContext.edgeLen(); y++) {
		for (uint32_t x=0; x<mContext.edgeLen(); x++) {
			const float xfrac = float(x) * fracStep;
			const float yfrac = float(y) * fracStep;
			assert(xfrac<=1.0 && yfrac<=1.0);
			const glm::vec3 p = GetSpherePoint(xfrac, yfrac);
			*(vts++) = p;

			*(pUV++) = xfrac;
			*(pUV++) = yfrac;
		}
	}
	assert(vts == &mContext.vertexs()[mContext.NUM_MESH_VERTS()]);
	assert(pUV == &mContext.uvs()[mContext.NUM_MESH_VERTS()*2]);

	// now create the VBO
	assert(nullptr==mVBO);
	mVBO = new CGLvbo( mContext.NUM_MESH_VERTS(), &mContext.vertexs()[0], nullptr, mContext.uvs() );
}

#ifdef _DEBUG
void GeoPatch::CheckEdgeFriendsHeightmaps() const {
	////////////////////////////////////////////////////////////////
	// useful debugging stuff and one-time initialisation
	static float *heightmapA = nullptr;
	static float *heightmapB = nullptr;
	static float *pEf = nullptr;
	static const uint32_t texDim = mContext.edgeLen();
	if(nullptr==heightmapA) {
		heightmapA = new float[texDim*texDim];
		heightmapB = new float[texDim*texDim];
		pEf = new float[texDim];
		for(uint32_t i=0;i<texDim;i++) {
			for(uint32_t j=0;j<texDim;j++) {
				heightmapA[i + (j*texDim)] = -1.0f;
				heightmapB[i + (j*texDim)] = -2.0f;
			}
			pEf[i] = -5.0f;
		}
	}
			
	////////////////////////////////////////////////////////////////
	//
	int we_are[4];
	for (int i=0; i<4; i++) {
		we_are[i] = edgeFriend[i]->GetEdgeIdxOf(this);
	}

	// get heightmapA's data
	GetHeightmapData(getHeightmapID(),heightmapA);

	for (int i=0; i<4; i++) {
		const GeoPatch *e = edgeFriend[i];

		// get heightmapB's data
		GetHeightmapData(e->getHeightmapID(),heightmapB);

		// performed on neighbours data
		// copies neighbours data into pEf
		uint32_t x, y;
		switch(we_are[i])
		{
		case 0:
			for (x=0; x<texDim; x++)
				pEf[(texDim-1)-x] = heightmapB[x];
			break;
		case 1:
			x = texDim-1;
			for (y=0; y<texDim; y++) 
				pEf[(texDim-1)-y] = heightmapB[x + y*texDim];
			break;
		case 2:
			y = texDim-1;
			for (x=0; x<texDim; x++) 
				pEf[(texDim-1)-x] = heightmapB[(texDim-1)-x + y*texDim];
			break;
		case 3:
			for (y=0; y<texDim; y++) 
				pEf[(texDim-1)-y] = heightmapB[((texDim-1)-y)*texDim];
			break;
		default: 
			assert(false && "this shouldn't happen");	
			break;
		}

		const int faceIdx = getPatchFaceIdx();
		// performed on our data
		// compares neighbours data (in pEf) to our own edge data
		bool badVerts = false;
		switch(i)
		{
		case 0:
			for (x=0; x<texDim; x++) {
				const float a = pEf[x];
				const float b = heightmapA[x];
				badVerts = ( a!=b );
			}
			break;
		case 1:
			x = texDim-1;
			for (y=0; y<texDim; y++) {
				const float a = heightmapA[x + y*texDim];
				const float b = pEf[y];
				badVerts = ( a!=b );
			}
			break;
		case 2:
			y = texDim-1;
			for (x=0; x<texDim; x++) {
				const float a = heightmapA[x + y*texDim];
				const float b = pEf[(texDim-1)-x];
				badVerts = ( a!=b );
			}
			break;
		case 3:
			for (y=0; y<texDim; y++) {
				const float a = pEf[(texDim-1)-y];
				const float b = heightmapA[0 + y*texDim];
				badVerts = ( a!=b );
			}
			break;
		default: 
			assert(false && "this shouldn't happen");	
			break;
		}

		if( badVerts ) {
			printf("badVerts: faceIdx %i, depth %u, edge %i wrong\n",faceIdx, mDepth,i);
		}
	}
}

void GeoPatch::CheckEdgeFriendsEdgePositions() const {
	////////////////////////////////////////////////////////////////
	// useful debugging stuff and one-time initialisation
	static glm::vec3 *posMapA = nullptr;
	static glm::vec3 *posMapB = nullptr;
	static glm::vec3 *pEv = nullptr;
	static const uint32_t texDim = mContext.edgeLen();
	if(nullptr==posMapA) {
		posMapA = new glm::vec3[texDim*texDim];
		posMapB = new glm::vec3[texDim*texDim];
		pEv = new glm::vec3[texDim];
	}

	////////////////////////////////////////////////////////////////
	// populate the position map for THIS quad
	PopulateNormalisedPosMap(this, posMapA, texDim);
			
	////////////////////////////////////////////////////////////////
	//
	int we_are[4];
	for (int i=0; i<4; i++) {
		we_are[i] = edgeFriend[i]->GetEdgeIdxOf(this);
	}

	const int faceIdx = getPatchFaceIdx();
	for (int i=0; i<4; i++) {
		const GeoPatch *e = edgeFriend[i];

		////////////////////////////////////////////////////////////////
		// populate the position map for the edgeFriend[i] quad
		PopulateNormalisedPosMap(e, posMapB, texDim);

		// performed on neighbours data
		// copies neighbours data into pEf
		uint32_t x, y;
		switch(we_are[i])
		{
		case 0:
			for (x=0; x<texDim; x++)
				pEv[(texDim-1)-x] = posMapB[x];
			break;
		case 1:
			x = texDim-1;
			for (y=0; y<texDim; y++)
				pEv[(texDim-1)-y] = posMapB[x + y*texDim];
			break;
		case 2:
			y = texDim-1;
			for (x=0; x<texDim; x++) 
				pEv[(texDim-1)-x] = posMapB[(texDim-1)-x + y*texDim];
			break;
		case 3:
			for (y=0; y<texDim; y++) 
				pEv[(texDim-1)-y] = posMapB[((texDim-1)-y)*texDim];
			break;
		default: 
			assert(false && "this shouldn't happen");	
			break;
		}
			
		// performed on our data
		// compares neighbours data (in pEv) to our own edge data
		bool badPos = false;
		switch(i)
		{
		case 0:
			for (x=0; x<texDim; x++) {
				const glm::vec3 a = pEv[x];
				const glm::vec3 b = posMapA[x];
				badPos = ( a!=b );
				if(badPos) 
					printf("%f, ", b-a);
			}
			break;
		case 1:
			x = texDim-1;
			for (y=0; y<texDim; y++) {
				const glm::vec3 a = posMapA[x + y*texDim];
				const glm::vec3 b = pEv[y];
				badPos = ( a!=b );
				if(badPos) 
					printf("%f, ", b-a);
			}
			break;
		case 2:
			y = texDim-1;
			for (x=0; x<texDim; x++) {
				const glm::vec3 a = posMapA[x + y*texDim];
				const glm::vec3 b = pEv[(texDim-1)-x];
				badPos = ( a!=b );
				if(badPos) 
					printf("%f, ", b-a);
			}
			break;
		case 3:
			for (y=0; y<texDim; y++) {
				const glm::vec3 a = pEv[(texDim-1)-y];
				const glm::vec3 b = posMapA[0 + y*texDim];
				badPos = ( a!=b );
				if(badPos) 
					printf("%f, ", b-a);
			}
			break;
		}

		if( badPos ) {
			printf("\nbadPos: faceIdx %i, depth %u, edge %i wrong\n",faceIdx, mDepth,i);
		}
	}
}
#endif

void GeoPatch::Render()
{
	if (kids[0]) {
		for (int i=0; i<4; i++) {
			kids[i]->Render();
		}
	} 
	else 
	{
#if TEST_CASE
		//glm::vec4 patchColour(float(mDepth+1) * (1.0f/float(GEOPATCH_MAX_DEPTH)), 0.0f, float(GEOPATCH_MAX_DEPTH-mDepth) * (1.0f/float(GEOPATCH_MAX_DEPTH)), 1.0f);
		static const glm::vec4 patchColour[6] = {
			glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),	// problem child - red (meets purple & cyan)
			glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),	// green
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),	// blue
			glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),	// purple
			glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),	// cyan
			glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)	// problem child - yellow (meets purple & blue)
		};
		glUniform4fv(mContext.patchColourID(), 1, &patchColour[getPatchFaceIdx()][0]);
#else
		glm::vec4 patchColour(float(mDepth+1) * (1.0f/float(GEOPATCH_MAX_DEPTH)), 0.0f, float(GEOPATCH_MAX_DEPTH-mDepth) * (1.0f/float(GEOPATCH_MAX_DEPTH)), 1.0f);
		//glm::vec4 patchColour(1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4fv(mContext.patchColourID(), 1, &patchColour[0]);
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

void GeoPatch::LODUpdate(const glm::vec3 &campos) {
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
			const glm::vec3 v01	= glm::normalize(mV0+mV1);
			const glm::vec3 v12	= glm::normalize(mV1+mV2);
			const glm::vec3 v23	= glm::normalize(mV2+mV3);
			const glm::vec3 v30	= glm::normalize(mV3+mV0);
			const glm::vec3 cn	= glm::normalize(mCentroid);
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
#if defined(_DEBUG) && TEST_CASE
			//CheckEdgeFriendsHeightmaps();
			//CheckEdgeFriendsEdgePositions();
#endif
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
