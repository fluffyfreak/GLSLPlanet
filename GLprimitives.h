// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __glprimitives_h__
#define __glprimitives_h__

#include <cstdint>
#include "GLvbo.h"

// Include GLM
#include <glm/glm.hpp>

class CGLquad
{
private:
	CGLvbo mVBO;

public:
	CGLquad(const bool bNormals_, const bool bUVs_);
	~CGLquad();

	void Render() const;
};

class CGLcube
{
private:
	static const uint32_t scNumFaces = 6;
	CGLvbo *mpVBO[scNumFaces];
	GLuint	mFaceTextures[scNumFaces];
#ifdef _DEBUG
	glm::vec4 mColours[scNumFaces];
	GLuint mColourUniform;
#endif

public:
#ifdef _DEBUG
	CGLcube(const bool bNormals_, const bool bUVs_, const GLuint textures[scNumFaces], const glm::vec4 colours[scNumFaces], const GLuint colourUniform);
#else
	CGLcube(const bool bNormals_, const bool bUVs_, const GLuint textures[scNumFaces]);
#endif
	~CGLcube();

	void Render() const;
};

#endif // __glprimitives_h__