#include "PBO.h"



PBO::PBO() {
	glGenBuffers(1, &ID);
}

void PBO::AutoResize(int newWidth, int newHeight) {
	if (width == newWidth && height == newHeight) return;
	width = newWidth;
	height = newHeight;
	Bind();
	glBufferData(GL_PIXEL_PACK_BUFFER, 4 * width * height, nullptr, GL_STREAM_READ);
}

void PBO::Bind() {
	glBindBuffer(GL_PIXEL_PACK_BUFFER, ID);
}

void PBO::Unbind() {
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void PBO::Delete() {
	glDeleteBuffers(1, &ID);
}