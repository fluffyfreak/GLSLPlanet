// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "GLfbo.h"
#include "glee.h"
#include "utils.h"
#include <cassert>


CGLfbo::CGLfbo(const uint32_t width, const uint32_t height) 
	: mWidth(width), mHeight(height)
{
	// 1) Create a framebuffer object 
	glGenFramebuffers(1, &mFBO);

	// 2) Bind the framebuffer object 
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// Now we need to create the texture which will contain the RGB output of our shader. 
	// This code is very classic :

	// The texture we're going to render to
	glGenTextures(1, &mTexture);
	checkGLError();
 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, mTexture);
	checkGLError();
 
	// Give an empty image to OpenGL ( the last "0" )
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWidth, mHeight, 0, GL_RED, GL_FLOAT, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWidth, mHeight, 0, GL_LUMINANCE, GL_FLOAT, 0);
	checkGLError();
 
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	checkGLError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);		//GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		//GL_CLAMP);
	checkGLError();

	// 3) Attach a texture to the FBO 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);
	checkGLError();

	// Set the list of draw buffers.
	//GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, 0};
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	//checkGLError();

	// Always check that our framebuffer is ok
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	Release();
}

CGLfbo::~CGLfbo() {
	if(glIsTexture(mTexture)==GL_TRUE) {
		glDeleteTextures(1, &mTexture);
		checkGLError();
	}
	if(glIsFramebuffer(mFBO)==GL_TRUE) {
		glDeleteFramebuffers(1, &mFBO);
		checkGLError();
	}
}

void CGLfbo::Bind() const {
	// Activate the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	checkGLError();
}

void CGLfbo::Release() const {
	// Disable the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError();
}

//void CGLfbo::GetData(float *data) const {
//	glBindTexture(GL_TEXTURE_2D, mTexture);
//	checkGLError();
//	glGetTexImage(GL_TEXTURE_2D,0,GL_LUMINANCE,GL_FLOAT,&data[0]);
//	checkGLError();
//	glBindTexture(GL_TEXTURE_2D, 0);
//	checkGLError();
//}

void CGLfbo::CopyTexture(const uint32_t target) const
{
	glBindTexture(GL_TEXTURE_2D, target);
	checkGLError();
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mWidth, mHeight, 0);
	checkGLError();
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGLError();
}
