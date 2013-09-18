// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_mix_h__
#define __node_mix_h__

#include "node.h"

class CNodeMix : public CNode
{
public:
	CNodeMix(const std::string &nodeID, const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeID, nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeMix() 
	{
	}

private:
};

#endif // __node_mix_h__
