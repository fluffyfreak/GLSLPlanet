// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <cassert>
#include "GLvbo.h"
#include "utils.h"

// Include GLM
#include <glm/glm.hpp>

//#define UINT_MAX (unsigned int (-1))

CGLvbo::CGLvbo(const int nElements, const void *pVertexBuf, const void *pNormalBuf, const void *pUVBuf, GLenum nUsage)
{
	assert(nullptr!=pVertexBuf);
	assert(0<nElements);
	assert( nUsage>=0x88E0 && nUsage<=0x88EA );

	mVAO = UINT_MAX;
	mVertObjId = UINT_MAX;
	mNormObjId = UINT_MAX;
	mUVObjId = UINT_MAX;

	Init(nElements, pVertexBuf, pNormalBuf, pUVBuf, nUsage);
}
CGLvbo::~CGLvbo()
{
	Cleanup();
}

// private
void CGLvbo::Init(const int nElements, const void *pVertexBuf, const void *pNormalBuf, const void *pUVBuf, GLenum nUsage)
{
	assert(nullptr!=pVertexBuf);
	assert(0<nElements);
	assert( nUsage>=0x88E0 && nUsage<=0x88EA );

	Cleanup();
	checkGLError();

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// the vertex buffer is NOT optional
	glGenBuffers(1, &mVertObjId);
	glBindBuffer(GL_ARRAY_BUFFER, mVertObjId);
	glBufferData(GL_ARRAY_BUFFER, nElements * sizeof(glm::vec3), pVertexBuf, nUsage);
	checkGLError();

	if(pNormalBuf) {
		glGenBuffers(1, &mNormObjId);
		glBindBuffer(GL_ARRAY_BUFFER, mNormObjId);
		glBufferData(GL_ARRAY_BUFFER, nElements * sizeof(glm::vec3), pNormalBuf, nUsage);
		checkGLError();
	}

	if(pUVBuf) {
		glGenBuffers(1, &mUVObjId);
		glBindBuffer(GL_ARRAY_BUFFER, mUVObjId);
		glBufferData(GL_ARRAY_BUFFER, nElements * sizeof(glm::vec2), pUVBuf, nUsage);
		checkGLError();
	}

	// LOG_GLERROR();
}

void CGLvbo::Cleanup()
{
	checkGLError();
	if(mVertObjId != UINT_MAX)
	{
		const GLboolean glbIsBuffer = glIsBuffer(mVertObjId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mVertObjId);
			mVertObjId = UINT_MAX;
		}
		checkGLError();
	}
	if(mNormObjId != UINT_MAX)
	{
		const GLboolean glbIsBuffer = glIsBuffer(mNormObjId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mNormObjId);
			mNormObjId = UINT_MAX;
		}
		checkGLError();
	}
	if(mUVObjId != UINT_MAX)
	{
		const GLboolean glbIsBuffer = glIsBuffer(mUVObjId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mUVObjId);
			mUVObjId = UINT_MAX;
		}
		checkGLError();
	}
	if(mVAO != UINT_MAX)
	{
		const GLboolean glbIsVA = glIsVertexArray(mVAO);
		if(glbIsVA==GL_TRUE) {
			glDeleteVertexArrays(1, &mVAO);
			mVAO = UINT_MAX;
		}
		checkGLError();
	}
}

void CGLvbo::Bind() const
{
	glBindVertexArray(mVAO);
	checkGLError();

	GLuint attributeIndex = 0;

	if(mVertObjId < UINT_MAX)
	{
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(attributeIndex);
		checkGLError();
		glBindBuffer(GL_ARRAY_BUFFER, mVertObjId);
		checkGLError();
		glVertexAttribPointer(
			attributeIndex,     // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			nullptr             // array buffer offset
		);
		checkGLError();
		++attributeIndex;
	}
	if(mNormObjId < UINT_MAX)
	{
		// 2nd attribute buffer : normals
		glEnableVertexAttribArray(attributeIndex);
		checkGLError();
		glBindBuffer(GL_ARRAY_BUFFER, mNormObjId);
		checkGLError();
		glVertexAttribPointer(
			attributeIndex,                   // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			nullptr                           // array buffer offset
		);
		checkGLError();
		++attributeIndex;
	}
	if(mUVObjId < UINT_MAX)
	{
		// 3rd attribute buffer : UVs
		glEnableVertexAttribArray(attributeIndex);
		checkGLError();
		glBindBuffer(GL_ARRAY_BUFFER, mUVObjId);
		checkGLError();
		glVertexAttribPointer(
			attributeIndex,                   // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			nullptr                           // array buffer offset
		);
		checkGLError();
		++attributeIndex;
	}
}

void CGLvbo::Release() const
{
	GLuint attributeIndex = 0;
	if(mVertObjId != UINT_MAX) {
		glDisableVertexAttribArray(attributeIndex);
		checkGLError();
		++attributeIndex;
	}
	if(mNormObjId != UINT_MAX) {
		glDisableVertexAttribArray(attributeIndex);
		checkGLError();
		++attributeIndex;
	}
	if(mUVObjId != UINT_MAX) {
		glDisableVertexAttribArray(attributeIndex);
		checkGLError();
		++attributeIndex;
	}

	glBindVertexArray(0);
	checkGLError();
}