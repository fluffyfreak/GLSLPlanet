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
							const uint32_t depth_,
							const GeoPatchID patchID_)
							: v0(v0_), v1(v1_), v2(v2_), v3(v3_), depth(depth_), patchID(patchID_)
	{
	}

	const glm::vec3 v0;
	const glm::vec3 v1;
	const glm::vec3 v2;
	const glm::vec3 v3;
	const uint32_t depth;
	const GeoPatchID patchID;
};

struct SSplitResult {
	SSplitResult(const glm::vec3 &v0_,
				const glm::vec3 &v1_,
				const glm::vec3 &v2_,
				const glm::vec3 &v3_,
				const uint32_t depth_,
				const GeoPatchID patchID_)
				: v0(v0_), v1(v1_), v2(v2_), v3(v3_), depth(depth_), patchID(patchID_)
	{
	}
	GLuint texID;
	const glm::vec3 v0;
	const glm::vec3 v1;
	const glm::vec3 v2;
	const glm::vec3 v3;
	const uint32_t depth;
	const GeoPatchID patchID;
};

class GeoSphere
{
private:
	void BuildFirstPatches();

	static const uint32_t NUM_PATCHES = 6;
	GeoPatch*			mGeoPatches[NUM_PATCHES];
	GeoPatchContext*	mGeoPatchContext;

	static const uint32_t MAX_SPLIT_REQUESTS = 128;
	std::deque<SSplitRequestDescription> mSplitRequestDescriptions;
	std::deque<SSplitResult> mSplitResult;

public:
	GeoSphere();
	~GeoSphere();

	void Update(const glm::vec3 &campos);
	void Render(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP);

	bool AddSplitRequest(const SSplitRequestDescription &desc);
	void ProcessSplitRequests(const uint32_t processNumRequests = 4);
	void ProcessSplitResults();

#ifdef _DEBUG
	CGLquad *mpUVquad;
	CGLcube *mpCube;
	GLuint simple_shader;
	GLuint simple_MatrixID;
	GLuint simple_ViewMatrixID;
	GLuint simple_ModelMatrixID;
	GLuint simple_texHeightmap;
	GLuint simple_colour;
	void SetupDebugRendering();
	void RenderCube(glm::mat4 &ViewMatrix);
	void RenderLOD0Heightmaps();
#endif
};

#endif //__TERRAINMESH_H__