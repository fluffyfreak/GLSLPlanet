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
using namespace glm;

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
#if TEST_CASE
	mGeoPatchContext = new GeoPatchContext(9);
#else
	mGeoPatchContext = new GeoPatchContext(29);//33);
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