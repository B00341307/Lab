#pragma once


#include <GLFW/glfw3.h>
#include "Texture.h"
#include<iostream>
#include <string>

class Sprite
{
private:
	TextureGUI texture;
	float yPos;
	float xPos;
	float rotation;
	float xScale = 1;
	float yScale = 1;
	float Speed;
public:
	Sprite();
	Sprite(TextureGUI texture);
	Sprite(std::string ipath);
	Sprite(std::string ipath, float RxPos, float RyPos);

	void setPos(float x, float y);
	void setRotation(float i);
	void setScale(float x);
	void setScale(float x, float y);
	void setRotTo(float i);
	void incRot(float i);
	void MovePos(float deltaTime);
	void MovePos(float x, float y);
	void setSpeed(float s);
	void updateTexture(std::string path);

	void update();
	void render();






};