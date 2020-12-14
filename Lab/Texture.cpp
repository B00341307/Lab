#pragma once

#include "Texture.h"

TextureGUI::TextureGUI()
{
	id = -1;
}

TextureGUI::TextureGUI(int ID)
{
	id = ID;

	if (!textureParam())
	{
		std::cout << "ID not found" << std::endl;
	}



}

TextureGUI::TextureGUI(std::string local)
{
	
	id = SOIL_load_OGL_texture(local.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &id);
	if (!textureParam())
	{
		std::cout << local << "not found" << std::endl;
	}

}

bool TextureGUI::textureParam()
{

	if (id > 0)
	{
		int mipL = 0;
		glBindTexture(GL_TEXTURE_2D, id);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, mipL, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, mipL, GL_TEXTURE_HEIGHT, &height);
		return true;

	}
	else
	{
		return false;
	}


}

int TextureGUI::getID()
{
	return id;
}
int TextureGUI::getHeight()
{
	return height;
}
int TextureGUI::getWidth()
{
	return width;
}