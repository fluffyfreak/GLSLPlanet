// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "node.h"
#include "../utils.h"

void SetPair(intPair &pair, const std::string &name, const rapidjson::Value &val)
{
	const char* pName = name.c_str();
	if( val.HasMember(pName) && !val[pName].IsNull() ) {
		const int iVal = val[pName].GetInt();
		pair.first = true;
		pair.second = iVal;
	} else {
		pair.first = false;
		pair.second = 0;
	}
}

void SetPair(dblPair &pair, const std::string &name, const rapidjson::Value &val)
{
	const char* pName = name.c_str();
	if( val.HasMember(pName) && !val[pName].IsNull() ) {
		const double dbl = val[pName].GetDouble();
		pair.first = true;
		pair.second = dbl;
	} else {
		pair.first = false;
		pair.second = 0.0;
	}
}

void SetPair(vec3Pair &pair, const std::string &name, const rapidjson::Value &val)
{
	const char* pName = name.c_str();
	if( val.HasMember(pName) && !val[pName].IsNull() ) {
		if( val[pName].IsArray() )
		{
			glm::vec3 vec;
			int i=0;
			for( rapidjson::Document::ConstValueIterator iter = val[pName].Begin(), itEnd = val[pName].End(); iter!=itEnd; ++iter, ++i ) {
				assert(i<3);
				vec[i] = float((*iter).GetDouble());
			}
			
			pair.first = true;
			pair.second = vec;
			return;
		}
	}

	pair.first = false;
	pair.second = glm::vec3(0.0f);
}

void SetPair(strPair &pair, const std::string &name, const rapidjson::Value &val)
{
	const char* pName = name.c_str();
	if( val.HasMember(pName) && !val[pName].IsNull() ) {
		const std::string str = val[pName].GetString();
		pair.first = true;
		pair.second = str;
	} else {
		pair.first = false;
	}
}

void SetPair(booPair &pair, const std::string &name, const rapidjson::Value &val)
{
	const char* pName = name.c_str();
	if( val.HasMember(pName) && !val[pName].IsNull() ) {
		const bool boo = val[pName].GetBool();
		pair.first = true;
		pair.second = boo;
	} else {
		pair.first = false;
		pair.second = false;
	}
}

CNode::CNode(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) : mNodeTypeName(nodeTypeName) 
{
	// Now we need to create the texture which will contain the heightmap. 
	glGenTextures(1, &mTextureID);
	checkGLError();
 
	// Bind the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	checkGLError();
 
	// Create the texture itself without any data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, nullptr);
	checkGLError();
		
	// Bad filtering needed
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	checkGLError();
}

CNode::~CNode() 
{
	if(glIsTexture(mTextureID)==GL_TRUE) {
		glDeleteTextures(1, &mTextureID);
		checkGLError();
	}
}