// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __node_incline_h__
#define __node_incline_h__

#include "node.h"

class CNodeIncline : public CNode
{
public:
	CNodeIncline(const std::string &nodeTypeName, const TParameterNode &parameters, const TSourcesNode &sources) 
		: CNode(nodeTypeName, parameters, sources)
	{
	}
	virtual ~CNodeIncline() 
	{
	}

private:
};

#endif // __node_incline_h__
