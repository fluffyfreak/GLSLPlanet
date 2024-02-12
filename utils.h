// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __utils_h__
#define __utils_h__

#include <cstdio>

#include "glew.h"
#include <gl\GLU.h>

void checkGLError();

template<class T> inline const T& Clamp(const T& x, const T& min, const T& max) { return x > max ? max : (x < min ? min : x); }

template<class T> 
class ScopedBindRelease {
	const T& mObj;
public:
	ScopedBindRelease(const T& obj) : mObj(obj) { mObj.Bind(); }
	~ScopedBindRelease() { mObj.Release(); }
};


#endif // __utils_h__

