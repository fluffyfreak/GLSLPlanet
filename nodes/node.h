// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_h__
#define __node_h__

#include "../rapidjson/document.h"
#include <glm/glm.hpp>
#include <utility>
#include <string>

typedef std::pair<bool, int> intPair;
typedef std::pair<bool, double> dblPair;
typedef std::pair<bool, glm::vec3> vec3Pair;
typedef std::pair<bool, std::string> strPair;
typedef std::pair<bool, bool> booPair;

struct TParameterNode
{
	dblPair factor;
	dblPair falloff;
    dblPair height;
    dblPair octaves;
    dblPair offset;
    dblPair size;
    dblPair step;
	dblPair hue;
    dblPair lightness;
    dblPair saturdation;
	dblPair repeat;
	booPair invert;
    booPair rough;
    booPair shallow;
    booPair slope;
};

struct TSourcesNode
{
	strPair alpha;
    strPair op1;
    strPair op2;
	strPair input;
    strPair weight;
};


struct TTerrainData {
	strPair input_height;
	strPair material;
	intPair offsetx;
	intPair offsety;
}; 

struct TViewportData {
	vec3Pair position;
	vec3Pair rotation;
}; 

struct TWorkspaceData {
	intPair offsetx;
	intPair offsety;
};

void SetPair(intPair &pair, const std::string &name, const rapidjson::Value &val);
void SetPair(dblPair &pair, const std::string &name, const rapidjson::Value &val);
void SetPair(vec3Pair &pair, const std::string &name, const rapidjson::Value &val);
void SetPair(strPair &pair, const std::string &name, const rapidjson::Value &val);
void SetPair(booPair &pair, const std::string &name, const rapidjson::Value &val);

class CNode
{
public:
	CNode(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) : mNodeTypeName(nodeTypeName) {}
	virtual ~CNode() {}

	const std::string &NodeTypeName() const { return mNodeTypeName; }
private:
	const std::string mNodeTypeName;
};

#endif // __node_h__
