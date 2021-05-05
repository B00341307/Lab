#define _USE_MATH_DEFINES

#include "AI.h"
#include <math.h>
#include <algorithm>
#include <iostream>


AI::AI(std::vector<glm::vec3*> x,glm::vec3 a,glm::vec3* b,Model* c,Shader* s)
{
	Checkpoints = x;
	EntPos = a;
	playerPosition_ = b;
	ModelPointer_ = c;
	ShaderPointer_ = s;

}
void AI::Update(double dt)
{
	glm::vec3 CheckDistance_ = *Checkpoints.at(CurrentTarget) - EntPos;
	float length = glm::length(CheckDistance_);
	if(length < 5)
	{
		CurrentTarget++;
		if(CurrentTarget == Checkpoints.size())
		{
			CurrentTarget = 0;
		}
		CheckDistance_ = *Checkpoints.at(CurrentTarget) - EntPos;
		std::cout << CurrentTarget << "\n";
		
	}

	
	
	//Calculate angle
	
	
	float dx = EntPos.x - Checkpoints.at(CurrentTarget)->x;
	float dz = EntPos.z - Checkpoints.at(CurrentTarget)->z;
	float angle =(float)(atan2(dx, dz) * 180/M_PI);
	
 	float distance = 0;
	//boost::algorithm::clamp(distance = CurrentSpeed * *deltatime/10, -10, 10);
	distance = CurrentSpeed * dt/1.1;
	Rotation = angle ;

	
	//Update Position
	float Dx = (float)distance * sin(Rotation + M_PI /2 );
	float Dz = (float)distance * cos(Rotation + M_PI  /2);
 	EntPos += glm::vec3(Dx, 0.f, Dz);

	

	glm::vec3 front;
	front.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
	front.y = sin(glm::radians(0.0f));
	front.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
	Front = glm::normalize(front);
	
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));


}
void AI::BeginDrawTranslate()
{
	ShaderPointer_->use();
	//ShaderPointer_->setMat4("projection", glm::mat4(1.f));
	//ShaderPointer_->setMat4("view", glm::mat4(1.f));
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, EntPos);
 	model = glm::rotate_slow(model, (Rotation), glm::vec3(0.f, 1.f, 0.f));
	model = glm::rotate(model, (float)M_PI, glm::vec3(0.f, 1.f, 0.f));
 	model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
	ShaderPointer_->setMat4("model", model);
	ModelPointer_->Draw(*ShaderPointer_);
}