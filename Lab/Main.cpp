#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include <vector>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(10.0f, 5.0f, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Player
bool pressed = false;
glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
float playerRotation = 0.0f;

float MovementSpeed = 20.0f;
float playerCurrentSpeed = 0.0f;

float TurnSpeed = 1.f;
float playerCurrentTurnSpeed = 0.0f;

float playerAcceleration = 10.0f;

glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Right = glm::normalize(glm::cross(Front, Up));
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");
    Shader shader2("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");
    Shader normalShader("Shaders/9.3.normal_visualization.vs", "Shaders/9.3.normal_visualization.fs", "Shaders/9.3.normal_visualization.gs");
    Shader shader_explosion("Shaders/9.2.geometry_shader.vs", "Shaders/9.2.geometry_shader.fs", "Shaders/9.2.geometry_shader.gs");

    // load models
    // -----------
    stbi_set_flip_vertically_on_load(true);
    Model RedCar("resources/objects/RedCarColours/RedCar.obj");
    Model RedCar2("resources/objects/RedCar/RedCar.obj");
;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----       
        processInput(window);
       // camera.Position = glm::vec3(playerPosition.x+10.0f, playerPosition.y+5.0f, playerPosition.z);
        camera.move(playerRotation, playerPosition, pressed);
        

        //move
        playerRotation += playerCurrentTurnSpeed * deltaTime;
        float distance = playerCurrentSpeed * deltaTime;
        float dx = (float)(distance * sin(playerRotation + M_PI/2));
        float dz = (float)(distance * cos(playerRotation + M_PI / 2));
        playerPosition += glm::vec3(dx, 0.0f, dz);

       // camera.Yaw = playerRotation;

      //  camera = glm::rotate(camera, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //camera.Front = camera.GetViewMatrixAtPlayer(playerPosition);
        
        //new front car
         // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
        front.y = sin(glm::radians(0.0f));
        front.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front,Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
        glm::mat4 view = camera.GetViewMatrixAtPlayer(playerPosition);


        //draw model using normal shader
       // playerRotation += M_PI / 2;
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPosition); // translate it down so it's at the center of the scene
        model = glm::rotate(model, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        RedCar.Draw(shader);



        //draw model2 using normal shader
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene       
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 3.0f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        RedCar2.Draw(shader);
  

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    //escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //forward / backward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        playerCurrentSpeed = playerCurrentSpeed - playerAcceleration * deltaTime;
        //playerCurrentSpeed = -MovementSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        playerCurrentSpeed = playerCurrentSpeed + (playerAcceleration * deltaTime) * 0.7f;
        //playerCurrentSpeed = MovementSpeed * 0.7f;
    }      
    else if (playerCurrentSpeed > 0.0f)
    {
        playerCurrentSpeed = playerCurrentSpeed - 25.0f * deltaTime;
        if (playerCurrentSpeed < 0.0f)
        {
            playerCurrentSpeed = 0.0f;
        }
    }
    else if (playerCurrentSpeed < 0.0f)
    {
        playerCurrentSpeed = playerCurrentSpeed + 25.0f * deltaTime;
        if (playerCurrentSpeed > 0.0f)
        {
            playerCurrentSpeed = 0.0f;
        }
    }



    //left / right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && playerCurrentSpeed<0.0f)
    {
        playerCurrentTurnSpeed = -0.05f * playerCurrentSpeed;
        pressed = true;
    }      
    else    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && playerCurrentSpeed < 0.0f)
    {
        playerCurrentTurnSpeed = 0.05f * playerCurrentSpeed;
        pressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && playerCurrentSpeed > 0.0f)
    {
        playerCurrentTurnSpeed = -0.05f * playerCurrentSpeed;
        pressed = true;
    }
    else    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && playerCurrentSpeed > 0.0f)
    {
        playerCurrentTurnSpeed = 0.05f * playerCurrentSpeed;
        pressed = true;
    }
    else
    {
        playerCurrentTurnSpeed = 0.0f;
        pressed = false;
    }

    //brake
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerCurrentSpeed > 0.0f)
    {
        playerCurrentSpeed = playerCurrentSpeed - 65.0f * deltaTime;

        if (playerCurrentSpeed < 0.0f)
        {
            playerCurrentSpeed = 0.0f;
        }
    }


     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerCurrentSpeed < 0.0f)
     {
        playerCurrentSpeed = playerCurrentSpeed + 65.0f * deltaTime;
        if (playerCurrentSpeed > 0.0f)
        {
            playerCurrentSpeed = 0.0f;
        }
     }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

  //  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //camera.ProcessMouseScroll(yoffset);
}