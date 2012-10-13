// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "shaderHelper.h"
#include "glee.h"
#include <gl\GLU.h>
#include <cstdio>

#include "utils.h"
#include "TextFile.h"

void LoadShader( GLuint &prog, std::string vertstr, std::string fragstr )
{
	GLuint v,f;
	std::string vs, fs;
	const int MaxInfoLogLength = 2048;
	GLchar infoLog[MaxInfoLogLength];
	GLsizei length=0;
	GLint param = GL_TRUE;

	const std::string shaderpath("./shaders/");

	/*const std::string libpath( shaderpath + "noise_lib.glsl");
	const std::string lib = textFileRead( libpath.c_str() );
	vs = lib;
	fs = lib;*/

	const std::string vertname( shaderpath + vertstr + ".vert");
	const std::string fragname( shaderpath + fragstr + ".frag");

	vs += textFileRead( vertname.c_str() );
	fs += textFileRead( fragname.c_str() );

	if( vs.size()>0 && fs.size()>0 )
	{
		v = glCreateShader(GL_VERTEX_SHADER);
		checkGLError();
		f = glCreateShader(GL_FRAGMENT_SHADER);
		checkGLError();

		const char * vv = vs.c_str();
		const char * ff = fs.c_str();;

		glShaderSource(v, 1, &vv,NULL);
		checkGLError();
		{
			glCompileShader(v);
			checkGLError();
			glGetShaderiv(v,GL_COMPILE_STATUS,&param);
			if( param==GL_FALSE ) {
				glGetShaderInfoLog(v,MaxInfoLogLength,&length,infoLog);
				printf("GLSL Error: %s", infoLog);
			}
			checkGLError();
		}

		glShaderSource(f, 1, &ff,NULL);
		checkGLError();
		{
			glCompileShader(f);
			checkGLError();
			glGetShaderiv(f,GL_COMPILE_STATUS,&param);
			if( param==GL_FALSE ) {
				glGetShaderInfoLog(f,MaxInfoLogLength,&length,infoLog);
				printf("GLSL Error: %s", infoLog);
			}
			checkGLError();
		}

		prog = glCreateProgram();
		checkGLError();
		glAttachShader(prog,v);
		checkGLError();
		glAttachShader(prog,f);
		checkGLError();

		glBindFragDataLocation(prog,0,"out_colour");

		glLinkProgram(prog);
		glGetProgramiv(prog,GL_LINK_STATUS,&param);
		if( param==GL_FALSE ) {
			glGetProgramInfoLog(prog,MaxInfoLogLength,&length,infoLog);
			printf("GLSL Link Error: %s", infoLog);
		}
		checkGLError();
	}
}
