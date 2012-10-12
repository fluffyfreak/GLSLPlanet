// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __TERRAINMESH_H__
#define __TERRAINMESH_H__

// Include GLEE
#include "glee.h"
#include <vector>

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

struct GLFramebufferObjectParams {
    int width, height, nColorAttachments, nSamples, nCSamples;
    bool hasDepth;
    GLenum format, depthFormat, type;
};

class GeoSphere
{
private:
	void BuildFirstPatches();

	static const uint32_t NUM_PATCHES = 6;
	GeoPatch*			mGeoPatches[NUM_PATCHES];
	GeoPatchContext*	mGeoPatchContext;

public:
	GeoSphere();
	~GeoSphere();

	void Update(const glm::vec3 &campos);
	void Render(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP);

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