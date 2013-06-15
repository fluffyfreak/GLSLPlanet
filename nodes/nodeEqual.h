// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_equal_h__
#define __node_equal_h__

#include "node.h"

class CNodeEqual : public CNode
{
public:
	CNodeEqual(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeEqual() 
	{
	}

private:
};

#endif // __node_equal_h__
