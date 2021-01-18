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

#include <ctime>

#include <chrono>

#include <cstdio>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);



// settings

const unsigned int SCR_WIDTH = 1600;

const unsigned int SCR_HEIGHT = 1000;

bool blinn = true;





// camera

Camera camera(glm::vec3(0.0f, 100.0f, 0.0f));

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





float MaxMovementSpeed = -60.0f;

float playerCurrentSpeed = 0.0f;



float TurnSpeed = 1.f;

float playerCurrentTurnSpeed = 0.0f;



float playerAcceleration = 10.0f;



glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);

glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 Right = glm::normalize(glm::cross(Front, Up));


//Rocks
glm::vec3 RockPosition[10];
float RockRotation[10];
float RockSpeed = 5.0f;
bool loaded = false;
bool exist[10] = { true,true,true,true,true,true,true,true,true,true };
bool reLoaded[10] = { true,true,true,true,true,true,true,true,true,true };
int i = 9;
double explosionTime[10];
glm::vec3 RockTempPos;

//explosion
double timer[10] = { -1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0 };


//Bullets

glm::vec3 BulletPosition[10];

float BulletRotation[10];

float BulletSpeed = 35.0f;

bool wasShoot[10];

float BulletCurrentSpeed[10];

bool canShoot = true;

int ReamainingLifeSpawn[10];

int BulletLifeSpawn = 20;





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

    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);



    // build and compile shaders

    // -------------------------



    Shader shader("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");

    Shader shader_explosion("Shaders/Geometry_shader.vs", "Shaders/Geometry_shader.fs", "Shaders/Geometry_shader.gs");

    Shader shaderSingleColor("Shaders/Stencil_testing.vs", "Shaders/Stencil_single_color.fs");

    Shader ToonTest("Shaders/ToonV3.vs", "Shaders/ToonV3.fs");







    // load models

    // -----------

    stbi_set_flip_vertically_on_load(true);

    Model Rocket("resources/objects/rocket4/rocket4.obj");

    Model Rock("resources/objects/rock/rock.obj");

    Model Bullet("resources/objects/bullet/bullet.obj");



    shader.use();

    shader.setInt("texture1", 0);



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





        //boundaries

        {

            if (playerPosition.x > 66.0f)//player

            {

                playerPosition.x = -66.0f;

            }

            else if (playerPosition.x < -66.0f)

            {

                playerPosition.x = 66.0f;

            }



            else if (playerPosition.z > 41.0f)

            {

                playerPosition.z = -41.0f;

            }

            else if (playerPosition.z < -41.0f)

            {

                playerPosition.z = 41.0f;

            }





            while (i > 0)//rocks

            {

                if (RockPosition[i].x > 66.0f)

                {

                    RockPosition[i].x = -66.0f;

                }

                else if (RockPosition[i].x < -66.0f)

                {

                    RockPosition[i].x = 66.0f;

                }



                else if (RockPosition[i].z > 41.0f)

                {

                    RockPosition[i].z = -41.0f;

                }

                else if (RockPosition[i].z < -41.0f)

                {

                    RockPosition[i].z = 41.0f;

                }

                i--;

            }

            i = 9;



            while (i > 0)//bullets

            {

                if (BulletPosition[i].x > 66.0f)

                {

                    BulletPosition[i].x = -66.0f;

                }

                else if (BulletPosition[i].x < -66.0f)

                {

                    BulletPosition[i].x = 66.0f;

                }



                else if (BulletPosition[i].z > 41.0f)

                {

                    BulletPosition[i].z = -41.0f;

                }

                else if (BulletPosition[i].z < -41.0f)

                {

                    BulletPosition[i].z = 41.0f;

                }

                i--;

            }

            i = 9;



        }

        //move

        {

            playerRotation += playerCurrentTurnSpeed * deltaTime;

            float distance = playerCurrentSpeed * deltaTime;

            float dx = (float)(distance * sin(playerRotation + M_PI / 2));

            float dz = (float)(distance * cos(playerRotation + M_PI / 2));

            playerPosition += glm::vec3(dx, 0.0f, dz);



            //move rocks

            if (loaded == true)

            {

                int i = 9;

                while (i > 0)

                {

                    float distance = RockSpeed * deltaTime;

                    float dx = (float)(distance * sin(RockRotation[i] * deltaTime + M_PI / 2));

                    float dz = (float)(distance * cos(RockRotation[i] * deltaTime + M_PI / 2));

                    RockPosition[i] += glm::vec3(dx, 0.0f, dz);

                    i--;

                }

            }



            i = 9;

            //move bullets

            if (loaded == true)

            {

                while (i > 0)

                {

                    if (wasShoot[i] == true)

                    {

                        float distance = BulletCurrentSpeed[i] * deltaTime;

                        float dx = (float)(distance * sin(BulletRotation[i] - M_PI / 2));

                        float dz = (float)(distance * cos(BulletRotation[i] - M_PI / 2));

                        BulletPosition[i] += glm::vec3(dx, 0.0f, dz);

                        ReamainingLifeSpawn[i] -= deltaTime;

                    }



                    i--;

                }

            }

            i = 9;



        }



        // render

        // ------

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!



         // set uniforms

        shaderSingleColor.use();

        glm::mat4 model = glm::mat4(1.0f);



        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        shaderSingleColor.setMat4("view", view);

        shaderSingleColor.setMat4("projection", projection);



        shader.use();

        shader.setMat4("view", view);

        shader.setMat4("projection", projection);



        glStencilMask(0x00);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);

        glStencilMask(0xFF);




    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, playerPosition); // translate it down so it's at the center of the scene
    model = glm::rotate(model, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model);
    Rocket.Draw(shader);

        i = 9;


        //draw rocks
        while (i > 0)
        {
            if (loaded == true && exist[i] == false) //Draw if is exploding
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, RockPosition[i]); // translate it down so it's at the center of the scene
                model = glm::rotate(model, RockRotation[i], glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	//scale


                 // explosion
                shader_explosion.use();
                shader_explosion.setMat4("projection", projection);
                shader_explosion.setMat4("view", view);
                shader_explosion.setMat4("model", model);



                shader_explosion.setFloat("time", timer[i]);
                timer[i] += deltaTime;

                Rock.Draw(shader_explosion);



            }
            else //draw if it is not exploding
            {
                ToonTest.use();

                ToonTest.setMat4("projection", projection);

                ToonTest.setMat4("view", view);

                ToonTest.setInt("texture", 0);

                ToonTest.setVec3("lightDir", glm::vec3(10, 0, 1));


                model = glm::mat4(1.0f);
                if (loaded == false)// || reLoaded[i] == false) //random position once
                {
                    if (rand() % 4 > 2)
                        RockTempPos.x = rand() % 66;
                    else
                        RockTempPos.x = (rand() % 66) * -1;

                    if (rand() % 4 > 2)
                        RockTempPos.z = rand() % 41;
                    else
                        RockTempPos.z = (rand() % 66) * -1;

                    RockTempPos.y = RockPosition[i].y;
                    RockPosition[i] = RockTempPos;
                    RockRotation[i] = (rand() % 40);
                    model = glm::rotate(model, RockRotation[i], glm::vec3(0.0f, 1.0f, 0.0f));

                    reLoaded[i] = true;
                }

                model = glm::translate(model, RockPosition[i]);
                model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	//scale
                ToonTest.setMat4("model", model);
                Rock.Draw(ToonTest);
            }
            i--;
        }

        i = 9;



        //draw bullets

        while (i > 0)

        {

            if (wasShoot[i] == true)

            {





                if (ReamainingLifeSpawn[i] > 0)

                {

                    shader.use();



                    model = glm::mat4(1.0f);

                    model = glm::translate(model, BulletPosition[i]);

                    model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));     //scale

                    model = glm::rotate(model, BulletRotation[i], glm::vec3(0.0f, 1.0f, 0.0f));

                    shader.setMat4("model", model);

                    Bullet.Draw(shader);



                }

                else

                {

                    wasShoot[i] = false;

                }



            }

            i--;

        }

        i = 9;





       
        //Rocks stencil buffer
        if (loaded == true)//stencil
        {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            shaderSingleColor.use();
            float scale = 1.4;

            while (i > 0)
            {
                model = glm::mat4(1.0f);

                if (loaded == false)// || reLoaded[i] == false)// i should not random location again but it does not works otherwise 
                {
                    if (rand() % 4 > 2)
                        RockTempPos.x = rand() % 66;
                    else
                        RockTempPos.x = (rand() % 66) * -1;

                    if (rand() % 4 > 2)
                        RockTempPos.z = rand() % 41;
                    else
                        RockTempPos.z = (rand() % 66) * -1;

                    RockTempPos.y = RockPosition[i].y;
                    RockPosition[i] = RockTempPos;

                    RockRotation[i] = (rand() % 40);
                    model = glm::rotate(model, RockRotation[i], glm::vec3(0.0f, 1.0f, 0.0f));

                    reLoaded[i] = true;


                }

                model = glm::translate(model, RockPosition[i]);

                if (exist[i] == false)//if is exploding hide stencil (scale to 0)                           
                    model = glm::scale(model, glm::vec3(0.0f, 0.0f, 0.0f));	//scale
                else
                    model = glm::scale(model, glm::vec3(0.3f * scale, 0.3f * scale, 0.3f * scale));	//scale
                shaderSingleColor.setMat4("model", model);
                Rock.Draw(shaderSingleColor);


                if (loaded == true && exist[i] == false && std::clock() > explosionTime[i] + 1000)//stop explosion after given time
                {
                    exist[i] = true;
                    timer[i] = -1.0;
                    reLoaded[i] = false;
                }

                i--;
            }
            glStencilMask(0xFF);

            glStencilFunc(GL_ALWAYS, 0, 0xFF);

            glEnable(GL_DEPTH_TEST);

            i = 9;
        }
        loaded = true;



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

    //forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (playerCurrentSpeed > MaxMovementSpeed)
            playerCurrentSpeed = playerCurrentSpeed - playerAcceleration * deltaTime;

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
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //left
    {
        playerCurrentTurnSpeed = TurnSpeed;
        pressed = true;
    }
    else    if (glfwGetKey(window, GLFW_KEY_D)) //right
    {
        playerCurrentTurnSpeed = -TurnSpeed;
        pressed = true;
    }
    else if (playerCurrentTurnSpeed > 0.0f) //drag 
    {
        playerCurrentTurnSpeed -= deltaTime;
        if (playerCurrentTurnSpeed < 0.0f)
        {
            playerCurrentTurnSpeed = 0.0f;
        }
        pressed = false;
    }
    else if (playerCurrentTurnSpeed < 0.0f) //drag
    {
        playerCurrentTurnSpeed += deltaTime;
        if (playerCurrentTurnSpeed > 0.0f)
        {
            playerCurrentTurnSpeed = 0.0f;
        }
        pressed = false;
    }


    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && canShoot == true) //SHOOT
    {
        canShoot = false;
        i = 9;
        while (i > 0)
        {
            if (wasShoot[i] == false)
            {
                BulletRotation[i] = playerRotation + playerCurrentTurnSpeed * deltaTime;
                BulletCurrentSpeed[i] = BulletSpeed - playerCurrentSpeed;
                BulletPosition[i] = playerPosition;

                wasShoot[i] = true;
                ReamainingLifeSpawn[i] = BulletLifeSpawn;
                i = -10;
            }
            i--;
        }
        i = 9;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) //SHOOT
    {
        canShoot = true;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && loaded == true) //explode asteroids test
    {
        i = 9;
        while (i > 0)
        {
            exist[i] = false;
            explosionTime[i] = std::clock();
            i--;
        }
        i = 9;
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

