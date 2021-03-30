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


void renderSphere();
int key = 0;


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


    // configure opengl state
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };


    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.v2.vs", "1.model_loading.v2.fs");
    Shader equirectangularToCubemapShader("2.2.2.cubemap.vs", "2.2.2.equirectangular_to_cubemap.fs");
    Shader backgroundShader("2.2.2.background.vs", "2.2.2.background.fs");

    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    backgroundShader.use();
    backgroundShader.setInt("environmentMap", 0);


    // load models
    // -----------
    Model victorianHouseModel("resources/objects/Victorian House/Victorian House 2 8 edit 2.obj");
    Model countryRoadModel("resources/objects/country road/terreno02.obj");
    stbi_set_flip_vertically_on_load(true);
    Model RedCar("resources/objects/RedCarColours/RedCar.obj");
    Model RedCar2("resources/objects/RedCar/RedCar.obj");

    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(("resources/hdr/kloppenheim_03_4k.hdr"), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderSphere();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    //brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    backgroundShader.use();
    backgroundShader.setMat4("projection", projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);



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

        // Draw Model
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            key = 1;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            key = 2;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            key = 3;
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            key = 4;
        }

        if (key == 1)
        {
            // == ==============================================================================================
            //       DESERT
            // == ==============================================================================================

            glClearColor(0.75f, 0.52f, 0.3f, 1.0f);

            glm::vec3 pointLightColors[] = {
                glm::vec3(1.0f, 0.6f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 1.0, 0.0),
                glm::vec3(0.2f, 0.2f, 1.0f)
            };

            // Directional light
            ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
            ourShader.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
            ourShader.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
            ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
            // Point light 1
            ourShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);

            ourShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);

            ourShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setFloat("pointLights[0].constant", 1.0f);

            ourShader.setFloat("pointLights[0].linear", 0.09);

            ourShader.setFloat("pointLights[0].quadratic", 0.032);
            // Point light 2

            ourShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);

            ourShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);

            ourShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setFloat("pointLights[1].constant", 1.0f);

            ourShader.setFloat("pointLights[1].linear", 0.09);

            ourShader.setFloat("pointLights[1].quadratic", 0.032);
            // Point light 3

            ourShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

            ourShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);

            ourShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setFloat("pointLights[2].constant", 1.0f);

            ourShader.setFloat("pointLights[2].linear", 0.09);

            ourShader.setFloat("pointLights[2].quadratic", 0.032);
            // Point light 4

            ourShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);

            ourShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);

            ourShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setFloat("pointLights[3].constant", 1.0f);

            ourShader.setFloat("pointLights[3].linear", 0.09);

            ourShader.setFloat("pointLights[3].quadratic", 0.032);
            // SpotLight

            ourShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);

            ourShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);

            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);

            ourShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f);

            ourShader.setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);

            ourShader.setFloat("spotLight.constant", 1.0f);

            ourShader.setFloat("spotLight.linear", 0.09);

            ourShader.setFloat("spotLight.quadratic", 0.032);

            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));

            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(13.0f)));
        }
        if (key == 2)
        {
            // == ==============================================================================================
            //       FACTORY
            // == ==============================================================================================

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            glm::vec3 pointLightColors[] = {
                glm::vec3(0.2f, 0.2f, 0.6f),
                glm::vec3(0.3f, 0.3f, 0.7f),
                glm::vec3(0.0f, 0.0f, 0.3f),
                glm::vec3(0.4f, 0.4f, 0.4f)
            };

            // Directional light

            ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

            ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);

            ourShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7);

            ourShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
            // Point light 1

            ourShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);

            ourShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);

            ourShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setFloat("pointLights[0].constant", 1.0f);

            ourShader.setFloat("pointLights[0].linear", 0.09);

            ourShader.setFloat("pointLights[0].quadratic", 0.032);
            // Point light 2

            ourShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);

            ourShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);

            ourShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setFloat("pointLights[1].constant", 1.0f);

            ourShader.setFloat("pointLights[1].linear", 0.09);

            ourShader.setFloat("pointLights[1].quadratic", 0.032);
            // Point light 3

            ourShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

            ourShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);

            ourShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setFloat("pointLights[2].constant", 1.0f);

            ourShader.setFloat("pointLights[2].linear", 0.09);

            ourShader.setFloat("pointLights[2].quadratic", 0.032);
            // Point light 4

            ourShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);

            ourShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);

            ourShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setFloat("pointLights[3].constant", 1.0f);

            ourShader.setFloat("pointLights[3].linear", 0.09);

            ourShader.setFloat("pointLights[3].quadratic", 0.032);
            // SpotLight

            ourShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);

            ourShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);

            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);

            ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);

            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

            ourShader.setFloat("spotLight.constant", 1.0f);

            ourShader.setFloat("spotLight.linear", 0.009);

            ourShader.setFloat("spotLight.quadratic", 0.0032);

            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));

            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));
        }
        if (key == 3)
        {
            // == ==============================================================================================
            //       HORROR
            // == ==============================================================================================

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glm::vec3 pointLightColors[] = {
                glm::vec3(0.1f, 0.1f, 0.1f),
                glm::vec3(0.1f, 0.1f, 0.1f),
                glm::vec3(0.1f, 0.1f, 0.1f),
                glm::vec3(0.3f, 0.1f, 0.1f)
            };

            // Directional light

            ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

            ourShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);

            ourShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);

            ourShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
            // Point light 1

            ourShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);

            ourShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);

            ourShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setFloat("pointLights[0].constant", 1.0f);

            ourShader.setFloat("pointLights[0].linear", 0.14);

            ourShader.setFloat("pointLights[0].quadratic", 0.07);
            // Point light 2

            ourShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);

            ourShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);

            ourShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setFloat("pointLights[1].constant", 1.0f);

            ourShader.setFloat("pointLights[1].linear", 0.14);

            ourShader.setFloat("pointLights[1].quadratic", 0.07);
            // Point light 3

            ourShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

            ourShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);

            ourShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setFloat("pointLights[2].constant", 1.0f);

            ourShader.setFloat("pointLights[2].linear", 0.22);

            ourShader.setFloat("pointLights[2].quadratic", 0.20);
            // Point light 4

            ourShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);

            ourShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);

            ourShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setFloat("pointLights[3].constant", 1.0f);

            ourShader.setFloat("pointLights[3].linear", 0.14);

            ourShader.setFloat("pointLights[3].quadratic", 0.07);
            // SpotLight

            ourShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);

            ourShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);

            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);

            ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);

            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

            ourShader.setFloat("spotLight.constant", 1.0f);

            ourShader.setFloat("spotLight.linear", 0.09);

            ourShader.setFloat("spotLight.quadratic", 0.032);

            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));

            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        if (key == 4)
        {
            // == ==============================================================================================
            //       BIOCHEMICAL LAB
            // == ==============================================================================================

            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

            glm::vec3 pointLightColors[] = {
                glm::vec3(0.4f, 0.7f, 0.1f),
                glm::vec3(0.4f, 0.7f, 0.1f),
                glm::vec3(0.4f, 0.7f, 0.1f),
                glm::vec3(0.4f, 0.7f, 0.1f)
            };

            // Directional light

            ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

            ourShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);

            ourShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);

            ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
            // Point light 1

            ourShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);

            ourShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);

            ourShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);

            ourShader.setFloat("pointLights[0].constant", 1.0f);

            ourShader.setFloat("pointLights[0].linear", 0.07);

            ourShader.setFloat("pointLights[0].quadratic", 0.017);
            // Point light 2

            ourShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);

            ourShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);

            ourShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);

            ourShader.setFloat("pointLights[1].constant", 1.0f);

            ourShader.setFloat("pointLights[1].linear", 0.07);

            ourShader.setFloat("pointLights[1].quadratic", 0.017);
            // Point light 3

            ourShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

            ourShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);

            ourShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);

            ourShader.setFloat("pointLights[2].constant", 1.0f);

            ourShader.setFloat("pointLights[2].linear", 0.07);

            ourShader.setFloat("pointLights[2].quadratic", 0.017);
            // Point light 4

            ourShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);

            ourShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);

            ourShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);

            ourShader.setFloat("pointLights[3].constant", 1.0f);

            ourShader.setFloat("pointLights[3].linear", 0.07);

            ourShader.setFloat("pointLights[3].quadratic", 0.017);
            // SpotLight

            ourShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);

            ourShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);

            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);

            ourShader.setVec3("spotLight.diffuse", 0.0f, 1.0f, 0.0f);

            ourShader.setVec3("spotLight.specular", 0.0f, 1.0f, 0.0f);

            ourShader.setFloat("spotLight.constant", 1.0f);

            ourShader.setFloat("spotLight.linear", 0.07);

            ourShader.setFloat("spotLight.quadratic", 0.017);

            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.0f)));

            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(10.0f)));
        }

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
          glm::mat4 view = camera.GetViewMatrixAtPlayer(playerPosition);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);

        glm::mat4 victorianHouse = glm::mat4(1.0f);
        victorianHouse = glm::translate(victorianHouse, glm::vec3(0.0f, -1.75f, 0.0f));
        victorianHouse = glm::scale(victorianHouse, glm::vec3(0.05f, 0.05f, 0.05f));
        ourShader.setMat4("model", victorianHouse);
        victorianHouseModel.Draw(ourShader);

        //Road Loader
        int i = 0;
        int amount = 4;
        float offset = 25.0f;

        glm::mat4 countryRoad = glm::mat4(1.0f);
        countryRoad = glm::translate(countryRoad, glm::vec3(0.0f, -1.75f, 0.0f));
        countryRoad = glm::scale(countryRoad, glm::vec3(1.0f, 0.2f, 1.0f));
        ourShader.setMat4("model", countryRoad);
        countryRoadModel.Draw(ourShader);

        while (i != amount)
        {
            //pos
            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f + offset, 0.00f, 0.0f));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f - (offset), 0.00f, 0.0f));
            ourShader.setMat4("model", countryRoad);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f, 0.00f, 0.0f + (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f, 0.00f, 0.0f - (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f + offset, 0.00f, 0.0f + (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f - (offset), 0.00f, 0.0f - (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);

            //neg
            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f - offset, 0.00f, 0.0f));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f + (offset), 0.00f, 0.0f));
            ourShader.setMat4("model", countryRoad);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f, 0.00f, 0.0f - (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f, 0.00f, 0.0f + (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f - offset, 0.00f, 0.0f - (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f + (offset), 0.00f, 0.0f + (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);

            //mix
            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f + offset, 0.00f, 0.0f - (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f - (offset), 0.00f, 0.0f + (offset / 1.45)));

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f - offset, 0.00f, 0.0f + (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);
            countryRoadModel.Draw(ourShader);

            countryRoad = glm::translate(countryRoad, glm::vec3(0.0f + (offset), 0.00f, 0.0f - (offset / 1.45)));
            ourShader.setMat4("model", countryRoad);


            i++;
            offset = offset + 25.0f;
        }

        //Background Loader
        backgroundShader.use();

        backgroundShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        renderSphere();

        // configure transformation matrices
      //  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
      //  glm::mat4 view = camera.GetViewMatrixAtPlayer(playerPosition);


        //draw model using normal shader
       // playerRotation += M_PI / 2;
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPosition); // translate it down so it's at the center of the scene
        model = glm::rotate(model, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        RedCar.Draw(ourShader);



        //draw model2 using normal shader
       
  

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