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

void renderSphere();

void renderQuad();



// settings

const unsigned int SCR_WIDTH = 1600;

const unsigned int SCR_HEIGHT = 1000;

bool blinn = true;

bool bloom = true;

float exposure = 1.0f;




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

float BulletSpeed = 20.0f;

bool wasShoot[10];

float BulletCurrentSpeed[10];

bool canShoot = true;

int ReamainingLifeSpawn[10];

int BulletLifeSpawn = 60;





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

    Shader bloomshader("Shaders/7.bloom.vs", "Shaders/7.bloom.fs");

    Shader shaderLight("Shaders/7.bloom.vs", "Shaders/7.light_box.fs");

    Shader shaderBlur("Shaders/7.blur.vs", "Shaders/7.blur.fs");

    Shader shaderBloomFinal("Shaders/7.bloom_final.vs", "Shaders/7.bloom_final.fs");



    // load models

    // -----------

    stbi_set_flip_vertically_on_load(true);

    Model Rocket("resources/objects/rocket4/rocket4.obj");

    Model Rock("resources/objects/rock/rock.obj");

    Model Bullet("resources/objects/bullet/bullet.obj");



    // configure (floating point) framebuffers
    // ---------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }



    // lighting info
    // -------------
    // position
    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3(BulletPosition->x, BulletPosition->y, BulletPosition->z));
    // color
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(15.0f, 0.0f, 0.0f));



    // shader configuration
    // --------------------
    bloomshader.use();
    bloomshader.setInt("diffuseTexture", 0);
    shaderBlur.use();
    shaderBlur.setInt("image", 0);
    shaderBloomFinal.use();
    shaderBloomFinal.setInt("scene", 0);
    shaderBloomFinal.setInt("bloomBlur", 1);



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

                        float distance = BulletCurrentSpeed[i] * (deltaTime);

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



        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection1 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view1 = camera.GetViewMatrix();
        glm::mat4 model1 = glm::mat4(1.0f);

        i = 9;

        //draw bullets
        shaderLight.use();
        shaderLight.setMat4("projection", projection1);
        shaderLight.setMat4("view", view1);

        while (i > 0)
        {

            if (wasShoot[i] == true)
            {





                if (ReamainingLifeSpawn[i] > 0)
                {




                    model1 = glm::mat4(1.0f);

                    model1 = glm::translate(model1, BulletPosition[i]);

                    model1 = glm::scale(model1, glm::vec3(0.8f, 0.8f, 0.8f));     //scale

                    model1 = glm::rotate(model1, BulletRotation[i], glm::vec3(0.0f, 1.0f, 0.0f));

                    shaderLight.setMat4("model", model1);

                    shaderLight.setVec3("lightColor", lightColors[0]);

                    renderSphere();

                }

                else
                {
                    wasShoot[i] = false;
                }

            }
            i--;

        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);





        // 2. blur bright fragments with two-pass Gaussian Blur 
                    // --------------------------------------------------
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 20;
        shaderBlur.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloomFinal.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        shaderBloomFinal.setInt("bloom", bloom);
        shaderBloomFinal.setFloat("exposure", exposure);
        renderQuad();
        
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

                ToonTest.setVec3("lightDir", glm::vec3(playerPosition.x,playerPosition.y,playerPosition.z));


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

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}