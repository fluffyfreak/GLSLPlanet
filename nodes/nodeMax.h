// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_max_h__
#define __node_max_h__

#include "node.h"

class CNodeMax : public CNode
{
public:
	CNodeMax(const std::string &nodeID, const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeID, nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeMax() 
	{
	}

private:
};

#endif // __node_max_h__
