#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem(GLfloat Length)
{
	this->Length = Length;
	this->VertexCount = 6;
	this->BufferData = new GLfloat[36]
	{
		-this->Length / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, this->Length / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, -this->Length / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, this->Length / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -this->Length / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, this->Length / 2.0f, 0.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &(this->VAO));
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &(this->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), this->BufferData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void CoordinateSystem::Draw(Shader Shader) const
{
	glBindVertexArray(this->VAO);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, this->VertexCount);
	glBindVertexArray(0);
}

