// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __TERRAINPATCH_H__
#define __TERRAINPATCH_H__

#include "TerrainPatchID.h"

// fwd decl's
class CGLvbo;

class GeoPatch
{
private:
	////////////////////////////////////////////////////////////////
	// private members

	const GeoPatchContext &mContext;
	GeoSphere *mpGeoSphere;
	CGLvbo *mVBO;
	GLuint mHeightmap;
	const glm::vec3 mV0, mV1, mV2, mV3;	// corner vertices for the patch
	const glm::vec3 mClipCentroid;
	glm::vec3 mCentroid;
	const uint32_t mDepth;
	float mClipRadius;
	float mRoughLength;

	const GeoPatchID mPatchID;
	bool mHasSplitRequest;

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
	GeoPatch(const GeoPatchContext &context_, GeoSphere *pGeoSphere_, 
		const glm::vec3 &v0_, const glm::vec3 &v1_, const glm::vec3 &v2_, const glm::vec3 &v3_, 
		const uint32_t depth_, const GeoPatchID &ID_);

	// destructor
	~GeoPatch();

	// in patch surface coords, [0,1]
	inline glm::vec3 GetSpherePoint(const float x, const float y) const {
		return glm::normalize((mV0 + x*(1.0f-y)*(mV1-mV0) + x*y*(mV2-mV0) + (1.0f-x)*y*(mV3-mV0)));
	}

	// Generates full-detail vertices, and also non-edge normals and colors
	void GenerateMesh();

	void ReceiveHeightmaps(const SSplitResult &s1, const SSplitResult &s2, const SSplitResult &s3, const SSplitResult &s4);
	void ReceiveHeightmapTex(const GLuint tex);

	inline void OnEdgeFriendChanged(const int edge, GeoPatch *e) {
		edgeFriend[edge] = e;
	}

	inline void NotifyEdgeFriendSplit(GeoPatch *e) {
		if (!kids[0]) {
			return;
		}
		const int idx = GetEdgeIdxOf(e);
		const int we_are = e->GetEdgeIdxOf(this);
		// match e's new kids to our own... :/
		kids[idx]->OnEdgeFriendChanged(idx, e->kids[(we_are+1)%4]);
		kids[(idx+1)%4]->OnEdgeFriendChanged(idx, e->kids[we_are]);
	}

	inline void NotifyEdgeFriendDeleted(const GeoPatch *e) {
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

	inline GeoPatch *GetEdgeFriendForKid(const int kid, const int edge) const {
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

	inline GLuint determineIndexbuffer() const {
		return // index buffers are ordered by edge resolution flags
			(edgeFriend[0] ? 1u : 0u) |
			(edgeFriend[1] ? 2u : 0u) |
			(edgeFriend[2] ? 4u : 0u) |
			(edgeFriend[3] ? 8u : 0u);
	}

#ifdef _DEBUG
	//// in patch surface coords, [0,1]
	//inline glm::vec3 GetSpherePointDebug(const float x, const float y,
	//	const glm::vec3 &V0, const glm::vec3 &V1, const glm::vec3 &V2, const glm::vec3 &V3) const 
	//{
	//	return glm::normalize((V0 + x*(1.0f-y)*(V1-V0) + x*y*(V2-V0) + (1.0f-x)*y*(V3-V0)));
	//}

	//inline glm::vec3 AltGetSpherePointDebug(const float x, const float y,
	//	const glm::vec3 &V0, const glm::vec3 &V1, const glm::vec3 &V2, const glm::vec3 &V3) const 
	//{
	//	// lerp ( a + t * (b - a) )
	//	const glm::vec3 A = V0+x*(V1-V0);
	//	const glm::vec3 B = V3+x*(V2-V3);
	//	const glm::vec3 C = A+y*(B-A);
	//	return glm::normalize(C);
	//}

	static void PopulateNormalisedPosMap(const GeoPatch* e, glm::vec3 *posMap, const uint32_t texDim)
	{
		////////////////////////////////////////////////////////////////
		glm::vec3 *vts = &posMap[0];
		assert(nullptr!=vts);
		const float fracStep = 1.0f/float(texDim-1);
		for (uint32_t y=0; y<texDim; y++) {
			for (uint32_t x=0; x<texDim; x++) {
				const float xfrac = float(x) * fracStep;
				const float yfrac = float(y) * fracStep;
				assert(xfrac<=1.0f && yfrac<=1.0f);
				const glm::vec3 p = e->GetSpherePoint(xfrac, yfrac);
				*(vts++) = p;
			}
		}
		assert(vts == &posMap[texDim*texDim]);
	}

	static void GenerateEdgeHeights(const uint32_t edge, const GeoPatch* e, glm::vec3 *pEv, const uint32_t texDim, const float fracStep)
	{
		uint32_t x=0, y=0;
		switch(edge)
		{
		case 0:
			for (x=0; x<texDim; x++) {
				const float xfrac = float(x) * fracStep;
				const float yfrac = float(y) * fracStep;
				assert(xfrac>=0.0f && yfrac>=0.0f);
				assert(xfrac<=1.0f && yfrac<=1.0f);
				pEv[(texDim-1)-x] = e->GetSpherePoint(xfrac, yfrac);
			}
			break;
		case 1:
			x = texDim-1;
			for (y=0; y<texDim; y++) {
				const float xfrac = float(x) * fracStep;
				const float yfrac = float(y) * fracStep;
				assert(xfrac>=0.0f && yfrac>=0.0f);
				assert(xfrac<=1.0f && yfrac<=1.0f);
				pEv[(texDim-1)-y] = e->GetSpherePoint(xfrac, yfrac);
			}
			break;
		case 2:
			y = texDim-1;
			for (x=0; x<texDim; x++) {
				const float xfrac = float((texDim-1)-x) * fracStep;
				const float yfrac = float(y) * fracStep;
				assert(xfrac>=0.0f && yfrac>=0.0f);
				assert(xfrac<=1.0f && yfrac<=1.0f);
				pEv[(texDim-1)-x] = e->GetSpherePoint(xfrac, yfrac);
			}
			break;
		case 3:
			for (y=0; y<texDim; y++) {
				const float xfrac = float(x) * fracStep;
				const float yfrac = float((texDim-1)-y) * fracStep;
				assert(xfrac>=0.0f && yfrac>=0.0f);
				assert(xfrac<=1.0f && yfrac<=1.0f);
				pEv[(texDim-1)-y] = e->GetSpherePoint(xfrac, yfrac);
			}
			break;
		}
	}

	static void GetHeightmapData(const GLuint texID, float *dataOut)
	{
		glBindTexture(GL_TEXTURE_2D, texID);
		checkGLError();
		glGetTexImage(GL_TEXTURE_2D,0,GL_LUMINANCE,GL_FLOAT,&dataOut[0]);
		checkGLError();
		glBindTexture(GL_TEXTURE_2D, 0);
		checkGLError();
	}

	void CheckEdgeFriendsHeightmaps() const;
	void CheckEdgeFriendsEdgePositions() const;
#endif

	void Render();

	void LODUpdate(const glm::vec3 &campos);

#ifdef _DEBUG
	GLuint getHeightmapID() const { return mHeightmap; }
#endif
};

#endif //__TERRAINPATCH_H__