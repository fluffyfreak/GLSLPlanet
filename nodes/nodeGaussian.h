// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_gaussian_h__
#define __node_gaussian_h__

#include "node.h"

class CNodeGaussian : public CNode
{
public:
	CNodeGaussian(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeGaussian() 
	{
	}

private:
};

#endif // __node_gaussian_h__
