// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_subtract_h__
#define __node_subtract_h__

#include "node.h"

class CNodeSubtract : public CNode
{
public:
	CNodeSubtract(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeSubtract() 
	{
	}

private:
};

#endif // __node_subtract_h__
