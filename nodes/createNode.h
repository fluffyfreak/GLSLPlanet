// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __create_node_h__
#define __create_node_h__

#include "node.h"

#include "nodeSimplex.h"
#include "nodeMix.h"
#include "nodeColour.h"
#include "nodeSubtract.h"
#include "nodeAdjust.h"
#include "nodeErode.h"
#include "nodeAdd.h"
#include "nodeMultiply.h"
#include "nodeIncline.h"
#include "nodeMax.h"
#include "nodeGaussian.h"
#include "nodeEqual.h"
#include "nodeWind.h"

#include <map>

enum ENodeType {
	eTypeSimplex = 0,
	eTypeMix,
	eTypeColour,
	eTypeSubtract,
	eTypeAdjust,
	eTypeErode,
	eTypeAdd,
	eTypeMultiply,
	eTypeIncline,
	eTypeMax,
	eTypeGaussian,
	eTypeEqual,
	eTypeWind
};

static std::map<std::string, ENodeType> gs_ETypeMap;

static CNode* CreateNode(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources)
{
	// lazy create
	if( gs_ETypeMap.empty() )
	{
		gs_ETypeMap["Simplex"]	= eTypeSimplex;
		gs_ETypeMap["Mix"]		= eTypeMix;
		gs_ETypeMap["Color"]	= eTypeColour;
		gs_ETypeMap["Subtract"] = eTypeSubtract;
		gs_ETypeMap["Adjust"]	= eTypeAdjust;
		gs_ETypeMap["Erode"]	= eTypeErode;
		gs_ETypeMap["Add"]		= eTypeAdd;
		gs_ETypeMap["Multiply"] = eTypeMultiply;
		gs_ETypeMap["Incline"]	= eTypeIncline;
		gs_ETypeMap["Max"]		= eTypeMax;
		gs_ETypeMap["Gaussian"] = eTypeGaussian;
		gs_ETypeMap["Equal"]	= eTypeEqual;
		gs_ETypeMap["Wind"]		= eTypeWind;
	}
	const int type = gs_ETypeMap[nodeTypeName];
	CNode* pNode = nullptr;
	switch(type)
	{
		case eTypeSimplex:	pNode = new CNodeSimplex(nodeTypeName, parameters, sources); break;
		case eTypeMix:		pNode = new CNodeMix(nodeTypeName, parameters, sources); break;
		case eTypeColour:	pNode = new CNodeColour(nodeTypeName, parameters, sources); break;
		case eTypeSubtract: pNode = new CNodeSubtract(nodeTypeName, parameters, sources); break;
		case eTypeAdjust:	pNode = new CNodeAdjust(nodeTypeName, parameters, sources); break;
		case eTypeErode:	pNode = new CNodeErode(nodeTypeName, parameters, sources); break;
		case eTypeAdd:		pNode = new CNodeAdd(nodeTypeName, parameters, sources); break;
		case eTypeMultiply: pNode = new CNodeMultiply(nodeTypeName, parameters, sources); break;
		case eTypeIncline:	pNode = new CNodeIncline(nodeTypeName, parameters, sources); break;
		case eTypeMax:		pNode = new CNodeMax(nodeTypeName, parameters, sources); break;
		case eTypeGaussian: pNode = new CNodeGaussian(nodeTypeName, parameters, sources); break;
		case eTypeEqual:	pNode = new CNodeEqual(nodeTypeName, parameters, sources); break;
		case eTypeWind:		pNode = new CNodeWind(nodeTypeName, parameters, sources); break;
	}
	
	return pNode;
}

#endif // __create_node_h__
