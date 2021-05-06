#pragma once
#include <GLFW/glfw3.h>
class Controller
{
	const char* name;
	GLFWgamepadstate state;
public:
	bool Init(int i);
	void GetStateChange();

};

