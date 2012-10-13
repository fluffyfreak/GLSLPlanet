// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __glfbo_h__
#define __glfbo_h__

#include <cstdint>

class CGLfbo
{
private:
	uint32_t mFBO;
	uint32_t mTexture;
	const uint32_t mWidth;
	const uint32_t mHeight;

public:
	CGLfbo(const uint32_t width, const uint32_t height);
	~CGLfbo();

	void Bind() const;
	void Release() const;

	inline uint32_t Width()	const { return mWidth; }
	inline uint32_t Height()	const { return mHeight; }

	void GetData(float *data) const;
};

#endif // __glfbo_h__