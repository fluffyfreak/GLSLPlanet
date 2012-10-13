// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __utils_h__
#define __utils_h__

#include <cstdio>

#include "glee.h"
#include <gl\GLU.h>

inline void checkGLError()
{
	GLenum err = glGetError();
	if( err ) {
		const char * errmsg = (const char *)gluErrorString(err);
		if( errmsg == nullptr )
			printf("[%s line %d] GL Error: Unknown\n", __FILE__, __LINE__);
		else
			printf("[%s line %d] GL Error: %s\n", __FILE__, __LINE__, errmsg);
	}
}

template<class T> inline const T& Clamp(const T& x, const T& min, const T& max) { return x > max ? max : (x < min ? min : x); }

template<class T> 
class ScopedBindRelease {
	const T& mObj;
public:
	ScopedBindRelease(const T& obj) : mObj(obj) { mObj.Bind(); }
	~ScopedBindRelease() { mObj.Release(); }
};


#endif // __utils_h__

