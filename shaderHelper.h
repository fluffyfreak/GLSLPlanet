// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __shaderHelper_h__
#define __shaderHelper_h__

#include <string>
#include <vector>

enum EShaderBinding {
	eBothShaders = 0,
	eVertShader,
	eFragShader
};

typedef std::pair<std::string,EShaderBinding> ShaderBindPair;
typedef std::vector<ShaderBindPair> vecBindings;
static const vecBindings s_nullBindings;

bool LoadShader( unsigned int &prog, const std::string &vertstr, const std::string &fragstr, const vecBindings &includePaths = s_nullBindings );

#endif // __shaderHelper_h__

