#pragma once

#include <glad/glad.h>

class FrameBuffer
{
public:
	FrameBuffer(int width, int height, bool hasDepth = true, int numTexs = 1, GLenum attachments = GL_DEPTH_ATTACHMENT) :
		mWidth(width), mHeight(height), mNumTexs(numTexs), mFrameBuffer(0), mRenderBuffer(0)
	{
		mFrameBuffer = 0;
		mRenderBuffer = 0;
		if (mNumTexs == 1)
			init_render_target(attachments, hasDepth);
		else
			init_render_targets(&attachments);
	}

	~FrameBuffer();

	void bind_render_target() const;
	void init_render_targets(GLenum* attachments);
	void init_render_target(GLenum attachment, bool hasDepth);
	void change_render_buffer_storage(int width, int height);
	void bind_texture(GLuint mTextureID, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum textureTarget = GL_TEXTURE_2D, unsigned int mip = 0);
	static void bind_render_targer_reset() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	inline void bind() { glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer); }

public:
	GLuint mFrameBuffer;
	GLuint mRenderBuffer;
	int mWidth;
	int mHeight;
	int mNumTexs;
};