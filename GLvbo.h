// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __glvbo_h__
#define __glvbo_h__

// Include GLEE to get definitions for GLuint and GLenum... yes Andy you should do this cleaner!
#include "glee.h"

class CGLvbo
{
private:
	void Init(const int nElements, const void *pVertexBuf, const void *pNormalBuf=nullptr, const void *pUVBuf=nullptr, GLenum nUsage=GL_STATIC_DRAW);	// Or GL_DYNAMIC_DRAW
	void Cleanup();

protected:
	GLuint mVAO;
	GLuint mVertObjId;
	GLuint mNormObjId;
	GLuint mUVObjId;

public:
	CGLvbo(const int nElements, const void *pVertexBuf, const void *pNormalBuf=nullptr, const void *pUVBuf=nullptr, GLenum nUsage=GL_STATIC_DRAW);	// Or GL_DYNAMIC_DRAW
	~CGLvbo();

	void Bind() const;
	void Release() const;
};

#endif // __glvbo_h__
