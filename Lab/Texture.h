#pragma once
#include "GLFW/glfw3.h"
#include "soil.1.16.0\build\native\include\soil.h"
#include <iostream>
#include <string>
class TextureGUI
{
private:

	int id;
	int width;
	int height;
	bool textureParam();
	GLuint texture;
	char path;
public:



	TextureGUI();
	TextureGUI(int ID);
	TextureGUI(std::string local);
	int getID();
	int getWidth();
	int getHeight();





};