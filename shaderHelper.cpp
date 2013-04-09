// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "shaderHelper.h"
#include "glee.h"
#include <gl\GLU.h>
#include <cstdio>

#include <direct.h>

#include <sstream>

#include "utils.h"
#include "TextFile.h"

std::string GetBaseFilename(const char *filename)
{
    std::string fName(filename);
    size_t pos = fName.rfind(".");
    if(pos == std::string::npos)  //No extension.
        return fName;

    if(pos == 0)    //. is at the front. Not an extension.
        return fName;

    return fName.substr(0, pos);
}

const std::string RemovePath(const std::string& filename)
{
  const int sz = static_cast<int>(filename.size());
  const int path_sz_fwd = filename.rfind("/",filename.size());
  const int path_sz_bak = filename.rfind("\\",filename.size());
  const int path_sz = max(path_sz_fwd, path_sz_bak);
  if (path_sz == sz) 
	  return filename;
  return filename.substr(path_sz + 1,sz - 1 - path_sz);
}

bool LoadShader( unsigned int &prog, const std::string &vertstr, const std::string &fragstr, const vecBindings &includePaths /* = s_nullBindings */ )
{
	bool success = true;
	GLuint v,f;
	std::string vs("#version 120\n\n"), fs("#version 120\n\n");
	const int MaxInfoLogLength = 2048;
	GLchar infoLog[MaxInfoLogLength];
	GLsizei length=0;
	GLint param = GL_TRUE;

	std::stringstream log;

	const std::string shaderpath("./shaders/");

	log << "--------------------------\n" << 
		"Loading: \n -- vertex prog \"" << vertstr.c_str() << "\" \n and \n -- fragement prog \"" << fragstr.c_str() << "\"\n" <<
		"With " << includePaths.size() << " libraries:\n" << std::endl;

	std::string lastLibLoadedName = RemovePath(GetBaseFilename(vertstr.c_str())) + RemovePath(GetBaseFilename(fragstr.c_str()));
	vecBindings::const_iterator iter = includePaths.begin();
	while (iter!=includePaths.end())
	{
		log << " -- :\"" << (*iter).first.c_str() << "\"" << std::endl;
		const std::string libpath( shaderpath + (*iter).first );
		const std::string lib = textFileRead( libpath.c_str() );
		lastLibLoadedName = libpath;
		switch ((*iter).second)
		{
		case eBothShaders:
			vs += lib;
			fs += lib;
			break;
		case eVertShader:
			vs += lib;
			break;
		case eFragShader:
			fs += lib;
			break;
		}

		// Next!
		++iter;
	}

	const std::string vertname( shaderpath + vertstr);
	const std::string fragname( shaderpath + fragstr);

	vs += textFileRead( vertname.c_str() );
	fs += textFileRead( fragname.c_str() );

	if( vs.size()>0 && fs.size()>0 )
	{
		const int vsn = std::count(std::string::iterator(vs.begin()), std::string::iterator(vs.end()), '\n') + 1;
		const int fsn = std::count(std::string::iterator(fs.begin()), std::string::iterator(fs.end()), '\n') + 1;

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
				log << "GLSL Error compiling \"" << vertstr.c_str() << "\": \n" << infoLog << "\n";
				success = false;
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
				log << "GLSL Error compiling \"" << fragstr.c_str() << "\": \n" << infoLog << "\n";
				success = false;
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
		if( param==GL_FALSE && success ) {
			glGetProgramInfoLog(prog,MaxInfoLogLength,&length,infoLog);
			log << "GLSL Link Error with: \n -- vertex prog \"" << vertstr.c_str() << "\" and \n";
			log << " -- fragement prog \"" << fragstr.c_str() << "\": \n" << infoLog << "\n\n";
			success = false;
		}
		checkGLError();

		if(!success) {
			log << "Number of lines in: \n -- vertex prog \"" << vertstr.c_str() << "\" == \"" << vsn << "\" and in \n -- fragement prog \"" << fragstr.c_str() << "\" == \"" << fsn << "\" \n\n";
		}
	}

	const std::string outfile = RemovePath(GetBaseFilename(lastLibLoadedName.c_str()));
	const std::string logExt = success ? ".log" : ".err";
	
	mkdir("./logs");
	textFileWrite( ("./logs/"+outfile+logExt).c_str(), log.str().c_str() );

	return success;
}
