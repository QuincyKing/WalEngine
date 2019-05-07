#include "Quad.h"

unsigned int Quad::vao = 0;

void Quad::draw()
{
	if (vao != 0)
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

void Quad::render(const Shader &shader)
{
	if (!mShader.is_default())
	{
		mShader.use();
		draw();
	}
	else
	{
		shader.use();
		draw();
	}
}

void Quad::load()
{
	if (vao == 0)
	{
		float quadVertices[] = {
			-1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f,

			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};

		unsigned int quadVBO;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
}