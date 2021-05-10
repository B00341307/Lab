#include "Controller.h"
#include <iostream>
bool Controller::Init(int i)
{
	switch(i)
	{
	case 1:
		if(glfwJoystickIsGamepad(GLFW_JOYSTICK_1)==true)
		{
			std::cout << "Controller " << i << "is present name" << glfwGetGamepadName(GLFW_JOYSTICK_1) << std::endl;
			return true;
		}
		else
		{
			std::cout << "Controller" << i << " not found" << std::endl;
			return false;
		}
	case 3:
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2) == true)
		{
			std::cout << "Controller " << i << "is present name" << glfwGetGamepadName(GLFW_JOYSTICK_1) << std::endl;
			return true;
		}
		else
		{
			std::cout << "Controller" << i << " not found" << std::endl;
			return false;
		}
		break;
	default:
		break;
	}
	
	

	
}
