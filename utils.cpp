// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "utils.h"

#pragma optimize( "", off )
void checkGLError()
{
	GLenum err = glGetError();
	if( err ) {
		const char * errmsg = (const char *)gluErrorString(err);
		if( errmsg == nullptr )
			printf("GL Error: Unknown\n");
		else
			printf("GL Error: %s\n", errmsg);
	}
}