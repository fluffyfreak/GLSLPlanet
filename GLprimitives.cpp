// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "GLprimitives.h"
#include "GLee.h"
#include "utils.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

////////////////////////////////////////////////////////////////
// CGLquad related data and definitions
namespace QuadData {
	static const GLfloat s_vertex_buffer_data[18] = { 
		// triangle 1
		-1.0f,-1.0f, 0.0f, // v1
		 1.0f, 1.0f, 0.0f, // v3
		-1.0f, 1.0f, 0.0f, // v2
		// triangle 2
		 1.0f, 1.0f, 0.0f, // v1
		-1.0f,-1.0f, 0.0f, // v3
		 1.0f,-1.0f, 0.0f  // v2
	};
	static const GLfloat s_normal_buffer_data[18] = { 
		// triangle 1
		 0.0f, 0.0f, 1.0f, // v1
		 0.0f, 0.0f, 1.0f, // v3
		 0.0f, 0.0f, 1.0f, // v2
		// triangle 2
		 0.0f, 0.0f, 1.0f, // v1
		 0.0f, 0.0f, 1.0f, // v3
		 0.0f, 0.0f, 1.0f  // v2
	};
	static const GLfloat s_uv_buffer_data[12] = { 
		// triangle 1
		0.0f, 0.0f, // v1
		1.0f, 1.0f, // v3
		0.0f, 1.0f, // v2
		// triangle 2
		1.0f, 1.0f, // v1
		0.0f, 0.0f, // v3
		1.0f, 0.0f  // v2
	};
}; // namespace QuadData

CGLquad::CGLquad(const bool bNormals_, const bool bUVs_) 
	: mVBO(sizeof(QuadData::s_vertex_buffer_data)/sizeof(QuadData::s_vertex_buffer_data[0]), 
			&QuadData::s_vertex_buffer_data[0], 
			bNormals_	? &QuadData::s_normal_buffer_data[0]	: nullptr,	// normals are optional
			bUVs_		? &QuadData::s_uv_buffer_data[0]		: nullptr) 	// UVcoords are optional
{
}
CGLquad::~CGLquad() 
{
}

void CGLquad::Render() const
{
	ScopedBindRelease<CGLvbo> vbo(mVBO);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 2*3); // From index 0 to 2*3 -> 2 triangles
	checkGLError();
}

////////////////////////////////////////////////////////////////
// CGLcube related data and definitions
namespace CubeData {
	static GLfloat s_vertex_buffer_data[6][12] = {
		{   0.58f,   0.58f,   0.58f,  -0.58f,   0.58f,   0.58f,  -0.58f,  -0.58f,   0.58f,   0.58f,  -0.58f,   0.58f  },
		{   0.58f,  -0.58f,   0.58f,  -0.58f,  -0.58f,   0.58f,  -0.58f,  -0.58f,  -0.58f,   0.58f,  -0.58f,  -0.58f  },
		{   0.58f,   0.58f,   0.58f,   0.58f,  -0.58f,   0.58f,   0.58f,  -0.58f,  -0.58f,   0.58f,   0.58f,  -0.58f  },
		{  -0.58f,   0.58f,   0.58f,   0.58f,   0.58f,   0.58f,   0.58f,   0.58f,  -0.58f,  -0.58f,   0.58f,  -0.58f  },
		{  -0.58f,  -0.58f,   0.58f,  -0.58f,   0.58f,   0.58f,  -0.58f,   0.58f,  -0.58f,  -0.58f,  -0.58f,  -0.58f  },
		{   0.58f,  -0.58f,  -0.58f,  -0.58f,  -0.58f,  -0.58f,  -0.58f,   0.58f,  -0.58f,   0.58f,   0.58f,  -0.58f }
	};
	static const GLfloat s_normal_buffer_data[6][12] = {
		{ 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f},
		{ 0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f},
		{ 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f},
		{ 0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f}
	};
	static GLfloat s_uv_buffer_data[8] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
}; // namespace CubeData
#ifdef _DEBUG
CGLcube::CGLcube(const bool bNormals_, const bool bUVs_, const GLuint textures[scNumFaces], const glm::vec4 colours[scNumFaces], const GLuint colourUniform)
#else
CGLcube::CGLcube(const bool bNormals_, const bool bUVs_, const GLuint textures[scNumFaces])
#endif
{
	for(uint32_t i=0; i<scNumFaces; i++) {
		assert(GL_TRUE==glIsTexture(textures[i]));
		mFaceTextures[i] = textures[i];
	}
#ifdef _DEBUG
	for(uint32_t i=0; i<scNumFaces; i++) {
		mColours[i] = colours[i];
	}
	mColourUniform = colourUniform;
#endif

	for(uint32_t i=0; i<scNumFaces; i++) {
		mpVBO[i] = new CGLvbo( (sizeof(CubeData::s_vertex_buffer_data)/sizeof(CubeData::s_vertex_buffer_data[i])), 
			&CubeData::s_vertex_buffer_data[i], 
			bNormals_	? &CubeData::s_normal_buffer_data[0]	: nullptr, 
			bUVs_		? &CubeData::s_uv_buffer_data[0]		: nullptr);
	}
}
CGLcube::~CGLcube()
{
	for(uint32_t i=0; i<scNumFaces; i++)
	{
		delete mpVBO[i];
		mpVBO[i] = nullptr;
	}
}

void CGLcube::Render() const
{
	if(nullptr==mpVBO[0]) 
		return;

	// render each of the faces
	for (int i=0; i<scNumFaces; i++) {
		ScopedBindRelease<CGLvbo> vbo(*mpVBO[i]);
#ifdef _DEBUG
		glUniform4fv(mColourUniform, 1, &mColours[i][0]);
#endif

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mFaceTextures[i]);
		glDrawArrays(GL_QUADS, 0, 4);
	}
	checkGLError();
}
