// Copyright � 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __TERRAINPATCHID_H__
#define __TERRAINPATCHID_H__

#include <cstdint>

class GeoPatchID
{
private:
	const uint64_t mPatchID;
public:
	GeoPatchID(const uint64_t init) : mPatchID(init) {}
	GeoPatchID(const GeoPatchID &init) : mPatchID(init.mPatchID) {}

	static const uint64_t MAX_SHIFT_DEPTH = 61;

	uint64_t NextPatchID(const int depth, const int idx) const;
	int GetPatchIdx(const int depth) const;
	int GetPatchFaceIdx() const;
};

#endif //__TERRAINPATCHID_H__