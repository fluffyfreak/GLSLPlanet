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

#include "TerrainContext.h"
#include "TerrainPatch.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TerrainMesh::TerrainMesh() : mGeoPatchContext(nullptr)
{
	for (int i=0; i<NUM_PATCHES; i++) {
		mGeoPatches[i] = nullptr;
	}
}

TerrainMesh::~TerrainMesh()
{
	for (int i=0; i<NUM_PATCHES; i++) {
		delete mGeoPatches[i];
		mGeoPatches[i] = nullptr;
	}

	delete mGeoPatchContext;
	mGeoPatchContext = nullptr;
}

void TerrainMesh::Update(const glm::vec3 &campos)
{
	if(nullptr==mGeoPatches[0]) {
		BuildFirstPatches();
	} else if(mSplitRequestDescriptions.empty()) {
		ProcessSplitResults();
		for (int i=0; i<NUM_PATCHES; i++) {
			mGeoPatches[i]->LODUpdate(campos);
		}
	} else {
		ProcessSplitRequests();
	}
}

void TerrainMesh::Render(const glm::mat4 &ViewMatrix, const glm::mat4 &ModelMatrix, const glm::mat4 &MVP)
{
	// setup the basics for the patch shader,
	// individual patches will change settings to match their own parameters
	mGeoPatchContext->UsePatchShader(ViewMatrix, ModelMatrix, MVP);

#if TEST_CASE
	mGeoPatches[0]->Render();	// red
	mGeoPatches[2]->Render();	// blue
#else
	checkGLError();
	for (int i=0; i<NUM_PATCHES; i++) {
		mGeoPatches[i]->Render();
	}
#endif
}

bool TerrainMesh::AddSplitRequest(SSplitRequestDescription *desc)
{
	if(mSplitRequestDescriptions.size()<MAX_SPLIT_REQUESTS) {
		mSplitRequestDescriptions.push_back(desc);
		return true;
	}
	return false;
}

void TerrainMesh::ProcessSplitRequests()
{
	std::deque<SSplitRequestDescription*>::const_iterator iter = mSplitRequestDescriptions.begin();
	while (iter!=mSplitRequestDescriptions.end())
	{
		const SSplitRequestDescription* srd = (*iter);

		const glm::vec3 v01	= glm::normalize(srd->v0+srd->v1);
		const glm::vec3 v12	= glm::normalize(srd->v1+srd->v2);
		const glm::vec3 v23	= glm::normalize(srd->v2+srd->v3);
		const glm::vec3 v30	= glm::normalize(srd->v3+srd->v0);
		const glm::vec3 cn	= glm::normalize(srd->centroid);

		// 
		const glm::vec3 vecs[4][4] = {
			{srd->v0,	v01,	cn,		v30},
			{v01,		srd->v1, v12,	cn},
			{cn,		v12,	srd->v2, v23},
			{v30,		cn,		v23,	srd->v3}
		};

		SSplitResult *sr = new SSplitResult(srd->patchID.GetPatchFaceIdx(), srd->depth);
		for (int i=0; i<4; i++)
		{
			// Now we need to create the texture which will contain the heightmap. 
			GLuint texID;
			glGenTextures(1, &texID);
			checkGLError();
 
			// Bind the newly created texture : all future texture functions will modify this texture
			glBindTexture(GL_TEXTURE_2D, texID);
			checkGLError();
 
			// Create the texture itself without any data
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 
				mGeoPatchContext->fboWidth(), mGeoPatchContext->fboWidth(), 
				0, GL_LUMINANCE, GL_FLOAT, nullptr);
			checkGLError();
		
			// Bad filtering needed
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			checkGLError();

			// render the heightmap to a framebuffer.
			mGeoPatchContext->renderHeightmap(vecs[i][0], vecs[i][1], vecs[i][2], vecs[i][3], texID);

			sr->addResult(texID, vecs[i][0], vecs[i][1], vecs[i][2], vecs[i][3], srd->patchID.NextPatchID(srd->depth+1, i));
		}

		// store result
		mSplitResult.push_back( sr );

		// cleanup after ourselves
		delete srd;

		// next!
		++iter;
	}
	mSplitRequestDescriptions.clear();
}

void TerrainMesh::ProcessSplitResults()
{
	std::deque<SSplitResult*>::const_iterator iter = mSplitResult.begin();
	while(iter!=mSplitResult.end())
	{
		// finally pass SplitResults
		const SSplitResult *psr = (*iter);

		const int32_t faceIdx = psr->face;
		mGeoPatches[faceIdx]->ReceiveHeightmaps(psr);

		// tidyup
		delete psr;

		// Next!
		++iter;
	}
	mSplitResult.clear();
}


static const int geo_sphere_edge_friends[6][4] = {
	{ 3, 4, 1, 2 },
	{ 0, 4, 5, 2 },
	{ 0, 1, 5, 3 },
	{ 0, 2, 5, 4 },
	{ 0, 3, 5, 1 },
	{ 1, 4, 3, 2 }
};

void TerrainMesh::BuildFirstPatches()
{
	assert(nullptr==mGeoPatchContext);
#if TEST_CASE
	mGeoPatchContext = new TerrainPatchContext(9);
#else
	mGeoPatchContext = new TerrainPatchContext(29);
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

	const uint64_t maxShiftDepth = TerrainPatchID::MAX_SHIFT_DEPTH;

	mGeoPatches[0] = new TerrainPatch(*mGeoPatchContext, this, p1, p2, p3, p4, 0, (0i64 << maxShiftDepth));
	mGeoPatches[1] = new TerrainPatch(*mGeoPatchContext, this, p4, p3, p7, p8, 0, (1i64 << maxShiftDepth));
	mGeoPatches[2] = new TerrainPatch(*mGeoPatchContext, this, p1, p4, p8, p5, 0, (2i64 << maxShiftDepth));
	mGeoPatches[3] = new TerrainPatch(*mGeoPatchContext, this, p2, p1, p5, p6, 0, (3i64 << maxShiftDepth));
	mGeoPatches[4] = new TerrainPatch(*mGeoPatchContext, this, p3, p2, p6, p7, 0, (4i64 << maxShiftDepth));
	mGeoPatches[5] = new TerrainPatch(*mGeoPatchContext, this, p8, p7, p6, p5, 0, (5i64 << maxShiftDepth));

	for (int i=0; i<NUM_PATCHES; i++) {
		for (int j=0; j<4; j++) {
			mGeoPatches[i]->OnEdgeFriendChanged(j, mGeoPatches[geo_sphere_edge_friends[i][j]]);
		}
	}
	/*for (int i=0; i<NUM_PATCHES; i++) {
		mGeoPatches[i]->GenerateMesh();
	}*/
}

