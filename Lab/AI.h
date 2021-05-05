#pragma once
#include <glm/glm.hpp>
#include "Model.h"
#include <vector>

class AI
{
public:
	std::vector<glm::vec3*> Checkpoints;
	glm::vec3 EntPos;
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Right = glm::normalize(glm::cross(Front, Up));

	glm::vec3* playerPosition_;
	Model* ModelPointer_;
	Shader* ShaderPointer_;
	float Rotation = 0;
	float CurrentTurn = 5;
	float CurrentSpeed = 20;


	
	int CurrentTarget = 0;


	AI(std::vector<glm::vec3*> x, glm::vec3 a,glm::vec3* b,Model* c,Shader* s);
	void Update(double dt);
	void BeginDrawTranslate();

};

