// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_add_h__
#define __node_add_h__

#include "node.h"

class CNodeAdd : public CNode
{
public:
	CNodeAdd(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeAdd() 
	{
	}

private:
};

#endif // __node_add_h__
