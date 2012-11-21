// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __TERRAINMESH_H__
#define __TERRAINMESH_H__

// Include GLEE
#include "glee.h"

#include "TerrainPatchID.h"

#include <deque>

// Include GLM
#include <glm/glm.hpp>

static const uint32_t NUM_SIDES = 6;

// fwd decl'
class GeoPatch;
class GeoPatchContext;
#ifdef _DEBUG
	class CGLquad;
	class CGLcube;
#endif

struct SSplitRequestDescription {
	SSplitRequestDescription(const glm::vec3 &v0_,
							const glm::vec3 &v1_,
							const glm::vec3 &v2_,
							const glm::vec3 &v3_,
							const glm::vec3 &cn,
							const uint32_t depth_,
							const GeoPatchID &patchID_)
							: v0(v0_), v1(v1_), v2(v2_), v3(v3_), centroid(cn), depth(depth_), patchID(patchID_)
	{
	}

	const glm::vec3 v0;
	const glm::vec3 v1;
	const glm::vec3 v2;
	const glm::vec3 v3;
	const glm::vec3 centroid;
	const uint32_t depth;
	const GeoPatchID patchID;
};

struct SSplitResult {
	struct SSplitResultData {
		SSplitResultData(const GLuint texID_, const glm::vec3 &v0_, const glm::vec3 &v1_, const glm::vec3 &v2_, const glm::vec3 &v3_, const GeoPatchID &patchID_) :
			texID(texID_), v0(v0_), v1(v1_), v2(v2_), v3(v3_), patchID(patchID_)
		{
		}
		const GLuint texID;
		const glm::vec3 v0;
		const glm::vec3 v1;
		const glm::vec3 v2;
		const glm::vec3 v3;
		const GeoPatchID patchID;
	};

	SSplitResult(const int32_t face_, const uint32_t depth_) : face(face_), depth(depth_)
	{
	}

	void addResult(const GLuint tex, const glm::vec3 &v0_, const glm::vec3 &v1_, const glm::vec3 &v2_, const glm::vec3 &v3_, const GeoPatchID &patchID_)
	{
		data.push_back(SSplitResultData(tex, v0_, v1_, v2_, v3_, patchID_));
		assert(data.size()<=4);
	}

	const int32_t face;
	const uint32_t depth;
	std::deque<SSplitResultData> data;
};

class GeoSphere
{
private:
	void BuildFirstPatches();

	static const uint32_t NUM_PATCHES = 6;
	GeoPatch*			mGeoPatches[NUM_PATCHES];
	GeoPatchContext*	mGeoPatchContext;

	static const uint32_t MAX_SPLIT_REQUESTS = 128;
	std::deque<SSplitRequestDescription*> mSplitRequestDescriptions;
	std::deque<SSplitResult*> mSplitResult;

public:
	GeoSphere();
	~GeoSphere();

	void Update(const glm::vec3 &campos);
	void Render(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP);

	bool AddSplitRequest(SSplitRequestDescription *desc);
	void ProcessSplitRequests();
	void ProcessSplitResults();
};

#endif //__TERRAINMESH_H__