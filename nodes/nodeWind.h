// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_wind_h__
#define __node_wind_h__

#include "node.h"

class CNodeWind : public CNode
{
public:
	CNodeWind(const std::string &nodeID, const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeID, nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeWind() 
	{
	}

private:
};

#endif // __node_wind_h__
