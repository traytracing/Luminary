#include "Texture.h"

#include <iostream>
#include <stb_image.h>


Texture::Texture(const char* ShapeFile, GLuint TextureIndex) : TextureIndex(TextureIndex) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		stbi_set_flip_vertically_on_load(true);

		int channels;
		unsigned char* shapeData = stbi_load(ShapeFile, &Width, &Height, &channels, STBI_rgb_alpha);

		if (!shapeData) {
			std::cerr << "Failed to load image!\n";
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, shapeData);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(shapeData);
}



void Texture::SetTextureIndex(GLuint index) {
	TextureIndex = index;
};


void Texture::BindTex() {
	glActiveTexture(GL_TEXTURE0 + TextureIndex);
	glBindTexture(GL_TEXTURE_2D, ID);
};


void Texture::UnbindTex() {
	glActiveTexture(GL_TEXTURE0 + TextureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
};


void Texture::Delete() {
	glDeleteTextures(1, &ID);
};