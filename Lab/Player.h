#pragma once
#pragma once
#include <glm/glm.hpp>

class Player
{
public:
    bool pressed = false;
    glm::vec3 playerPosition = glm::vec3(10.0f, 0.0f, -33.0f); //-20
    float playerRotation = 1.5f;
    double MovementSpeed = 20.00000000000;
    double playerCurrentSpeed = 0.00000000000;
    double TurnSpeed = 1.00000;
    double playerCurrentTurnSpeed = 0.00000;
    double playerAcceleration = 100.000;

    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Right = glm::normalize(glm::cross(Front, Up));

    float playerMaxTurnSpeed = 2.7f;
    float playerMaxSpeed = 125.0f;

    int PlayerBoost = 0;
    float BoostTimer;

    int points = 0;
    int Laps = 0;

    void PlayerGotBoost()
    {     
        PlayerBoost++;
        BoostTimer = std::clock();
        playerMaxSpeed = 350.0f;
        playerCurrentSpeed += playerCurrentSpeed * PlayerBoost / 5;
    }

    void PlayerMove()
    {
        if (playerCurrentSpeed < -playerMaxSpeed)
            playerCurrentSpeed = -playerMaxSpeed;

        if (playerCurrentSpeed > playerMaxSpeed)
            playerCurrentSpeed = playerMaxSpeed;

        playerRotation += playerCurrentTurnSpeed / 100;
        double distance = playerCurrentSpeed / 100;// *deltaTime;
        double dx = (double)(distance * sin(playerRotation + M_PI / 2));
        double dz = (double)(distance * cos(playerRotation + M_PI / 2));
        playerPosition += glm::vec3(dx, 0.0f, dz);
    }

    void PlayerNewFront()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
        front.y = sin(glm::radians(0.0f));
        front.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
        Front = glm::normalize(front);

        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
   
};