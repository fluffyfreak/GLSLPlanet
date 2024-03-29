// Copyright � 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <cassert>

#include "TerrainPatchID.h"

static const uint32_t MAX_PATCH_DEPTH = 30;
uint64_t TerrainPatchID::NextPatchID(const int depth, const int idx) const
{
	assert(idx>=0 && idx<4);
	assert(depth<=MAX_PATCH_DEPTH);
	const uint64_t idx64 = idx;
	const uint64_t shiftDepth64 = depth*2ULL;
	assert((mPatchID & (3i64<<shiftDepth64))==0);
	return uint64_t( mPatchID | (idx64<<shiftDepth64) );
}

int TerrainPatchID::GetPatchIdx(const int depth) const
{
	assert(depth<=MAX_PATCH_DEPTH);
	const uint64_t shiftDepth64 = depth*2ULL;
	const uint64_t idx64 = (mPatchID & (3i64<<shiftDepth64)) >> shiftDepth64;
	assert(idx64<=uint64_t(-1));
	return int(idx64);
}

int TerrainPatchID::GetPatchFaceIdx() const
{
	const int res = (mPatchID & (7i64 << MAX_SHIFT_DEPTH)) >> MAX_SHIFT_DEPTH;
	assert(res>=0 && res<6);
	return res;
}
