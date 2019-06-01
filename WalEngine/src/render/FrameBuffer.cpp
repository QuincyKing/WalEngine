#include "FrameBuffer.h"
#include <cassert>

FrameBuffer::~FrameBuffer()
{
	if (mFrameBuffer) glDeleteFramebuffers(1, &mFrameBuffer);
	if (mRenderBuffer) glDeleteRenderbuffers(1, &mRenderBuffer);
}

void FrameBuffer::bind_render_target() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	glViewport(0, 0, mWidth, mHeight);
}

void FrameBuffer::init_render_targets(GLenum* attachments)
{
	if (*attachments == 0)
		return;

	GLenum drawBuffers[32];      //32 is the max number of bound textures in OpenGL

	bool hasDepth = false;
	for (int i = 0; i < mNumTexs; i++)
	{
		if (attachments[i] == GL_DEPTH_ATTACHMENT)
		{
			drawBuffers[i] = GL_NONE;
			//hasDepth = true;
		}
		else
			drawBuffers[i] = attachments[i];

		if (attachments[i] == GL_NONE)
			continue;

		if (mFrameBuffer == 0)
		{
			glGenFramebuffers(1, &mFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
		}
	}

	if (mFrameBuffer == 0)
		return;

	if (!hasDepth)
	{
		glGenRenderbuffers(1, &mRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
	}

	glDrawBuffers(mNumTexs, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		assert(false);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::init_render_target(GLenum attachment, bool hasDepth)
{
	if (attachment == 0)
		return;

	GLenum drawBuffer; 

	if (attachment == GL_DEPTH_ATTACHMENT)
		drawBuffer = GL_NONE;
	else
		drawBuffer = attachment;

	if (mFrameBuffer == 0)
	{
		glGenFramebuffers(1, &mFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	}

	if (mFrameBuffer == 0)
		return;

	if (hasDepth)
	{
		glGenRenderbuffers(1, &mRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mWidth, mHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
	}

	if(drawBuffer != GL_NONE)
		glDrawBuffers(mNumTexs, &drawBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		assert(false);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::change_render_buffer_storage(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
}

void FrameBuffer::bind_texture(GLuint mTextureID, GLenum attachment, GLenum textureTarget, unsigned int mip)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, mTextureID, mip);
}