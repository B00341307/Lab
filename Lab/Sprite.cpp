#pragma once

#include "Sprite.h"

Sprite::Sprite()
{
	xPos = 0;
	yPos = 0;
	rotation = 0;
	texture = TextureGUI();
	Speed = 100;
	xScale = 1;
	yScale = 1;
}

Sprite::Sprite(std::string ipath)
{
	texture = TextureGUI(ipath);
	xPos = 0;
	yPos = 0;
	rotation = 0;;
	Speed = 100;
	xScale = 1;
	yScale = 1;

}
Sprite::Sprite(std::string ipath, float RxPos, float RyPos)
{
	texture = TextureGUI(ipath);
	xPos = RxPos;
	yPos = RyPos;
	rotation = 0;;
	Speed = 100;
	xScale = 1;
	yScale = 1;


}

void Sprite::update()
{
	//rotation--;
}

void Sprite::render()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.getID());


	glLoadIdentity();
	//glPushMatrix();
	glPopMatrix();
	//Translate
	glTranslatef(xPos, yPos, 0); //change later for zs
	//Roate
	glRotatef(rotation, 0, 1, 0);
	//Scale
	glScalef(xScale, yScale, 1);
	//RenderLoop
	glColor4f(1, 1, 1, 1);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(1, 0);
	glVertex2f(texture.getWidth(), 0);
	glTexCoord2f(1, 1);
	glVertex2f(texture.getWidth(), texture.getHeight());
	glTexCoord2f(0, 1);
	glVertex2f(0, texture.getHeight());

	glEnd();
	glPushMatrix();
	glPopMatrix();

	//Prevent contridiction
	glDisable(GL_TEXTURE_2D);






}
void Sprite::setPos(float x, float y)
{
	glPushMatrix();
	xPos = x;
	yPos = y;
	glPopMatrix();
}
void Sprite::setRotation(float i)
{

	rotation = i;
}
void Sprite::setScale(float x)
{

	xScale = x;
	yScale = x;

}
void Sprite::setScale(float x, float y)
{

	glPushMatrix();
	xScale = x;
	yScale = y;
	glPopMatrix();

}

void Sprite::setRotTo(float i)
{
	rotation = i;
}
void Sprite::incRot(float i)
{
	rotation += i;
}
void Sprite::MovePos(float deltaTime)
{
	xPos = xPos + (Speed * deltaTime);
}
void Sprite::MovePos(float x, float y)
{
}
void Sprite::updateTexture(std::string path)
{
	texture = TextureGUI(path);

}
