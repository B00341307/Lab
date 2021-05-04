#include <glm/glm.hpp>

class Player
{
public:
    bool pressed = false;
    glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    float playerRotation = 0.0f;
    double MovementSpeed = 20.00000000000;
    double playerCurrentSpeed = 0.00000000000;
    double TurnSpeed = 1.00000;
    double playerCurrentTurnSpeed = 0.00000;
    double playerAcceleration = 100.000;

    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Right = glm::normalize(glm::cross(Front, Up));

    float playerMaxTurnSpeed = 3.2f;
};