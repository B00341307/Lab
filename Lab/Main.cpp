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
 
#define GLT_IMPLEMENTATION//does not works
#include "gltext.h"   //does not works

//#include "lodepng.h" //does not works

#include <GL/glu.h>

float numberOfPlayers = 1.0f;
bool PisPressed = false;

void glDisable2D();
void glEnable2D();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void renderSphere();
int key = 1;

double tempdp;
// settings                        2k:       wide 2k:  fullhd:
const unsigned int SCR_WIDTH =  /*2560;*/   /*3440;*/   1920;
const unsigned int SCR_HEIGHT = /*1440;*/   /*1440;*/   1080;

// camera
Camera camera(glm::vec3(15.0f, 5.0f, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

Camera camera2(glm::vec3(15.0f, 5.0f, 0.0f));
float lastX2 = (float)SCR_WIDTH / 2.0;
float lastY2 = (float)SCR_HEIGHT / 2.0;
bool firstMouse2 = true;


// timing
double deltaTime = 0.0000;
double lastFrame = 0.0000;


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
Player player1;
Player player2;

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
 //   glfwSetCursorPosCallback(window, mouse_callback);
  //  glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
  //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
  //  Shader shader("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");
 //  Shader shader2("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");
  //  Shader normalShader("Shaders/9.3.normal_visualization.vs", "Shaders/9.3.normal_visualization.fs", "Shaders/9.3.normal_visualization.gs");
  //  Shader shader_explosion("Shaders/9.2.geometry_shader.vs", "Shaders/9.2.geometry_shader.fs", "Shaders/9.2.geometry_shader.gs");


    Shader ourShader("1.model_loading.v2.vs", "1.model_loading.v2.fs");
    Shader equirectangularToCubemapShader("2.2.2.cubemap.vs", "2.2.2.equirectangular_to_cubemap.fs");
    Shader backgroundShader("2.2.2.background.vs", "2.2.2.background.fs");

    Shader toonShader("Shaders/Toon.vs", "Toon.fs");
  
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    backgroundShader.use();
    backgroundShader.setInt("environmentMap", 0);

    toonShader.use();
    toonShader.setInt("toon", 0);

    // load models
    // -----------
   
    Model victorianHouseModel("resources/objects/Victorian House/Victorian House 2 8 edit 2.obj");
    Model countryRoadModel("resources/objects/country road/terreno02.obj");

    stbi_set_flip_vertically_on_load(true);
    Model RedCar("resources/objects/RedCarColours/RedCar.obj");
    Model RedCar2("resources/objects/BlueCar/RedCar.obj");

    // hud
    Model Shield("resources/HUD/shield/shield.obj");

    Model track("resources/objects/track/track.obj");
   
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
    glm::mat4 projection2 = glm::perspective(glm::radians(camera2.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    backgroundShader.use();
    backgroundShader.setMat4("projection", projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);


    GLTtext* text1 = gltCreateText();
    gltSetText(text1, "Hello World!");

    GLTtext* text2 = gltCreateText();

    double time =1000;
    char str[30];
    /////////////////////////////////////////////////////////////////////////

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);





    // load and create a texture ////////////////////////////////////////////////////////////
// -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int tidth, theight, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    //unsigned char* tdata = stbi_load("resources\HUD\shield\shield.png", &tidth, &theight, &nrChannels, 0);
 //   float* tdata = stbi_loadf(("resources\HUD\shield\shield.png"), &tidth, &theight, &nrChannels, 0);
    unsigned char* tdata = stbi_load(("resources/HUD/shield/shield.png"), &tidth, &theight, &nrChannels, 0);
    if (tdata)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "QFailed to load texture" << std::endl;
    }
    stbi_image_free(tdata);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----     
        processInput(window);
        
        if (numberOfPlayers == 2.0f)
        {
            //top screen
            glViewport(0, SCR_HEIGHT / 2, SCR_WIDTH , SCR_HEIGHT / 2);
           /* glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);
            glOrtho(0, SCR_WIDTH, SCR_WIDTH, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);*/
            /*
            // setup your right view projection:
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH, SCR_HEIGHT);
            glOrtho(0, SCR_WIDTH, SCR_WIDTH, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            */
        }
        else
        {
            //normal screen
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
           /* glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glOrtho(0, SCR_WIDTH, SCR_WIDTH, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);*/
        }
      
        
         camera.Position = glm::vec3(player1.playerPosition.x+10.0f, 
                                     player1.playerPosition.y+5.0f,
                                     player1.playerPosition.z);


        camera.move(player1.playerRotation, player1.playerPosition, player1.pressed);

        //move
        if (player1.playerCurrentSpeed < -530)
            player1.playerCurrentSpeed = -530;

        if (player1.playerCurrentSpeed > 530)
            player1.playerCurrentSpeed = 530;

        player1.playerRotation += player1.playerCurrentTurnSpeed/100;
        double distance = player1.playerCurrentSpeed/100;// *deltaTime;
        double dx = (double)(distance * sin(player1.playerRotation + M_PI / 2));
        double dz = (double)(distance * cos(player1.playerRotation + M_PI / 2));
        player1.playerPosition += glm::vec3(dx, 0.0f, dz);

        // camera.Yaw = playerRotation;

     //  camera = glm::rotate(camera, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
       //camera.Front = camera.GetViewMatrixAtPlayer(playerPosition);

        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
        front.y = sin(glm::radians(0.0f));
        front.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
        player1.Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        player1.Right = glm::normalize(glm::cross(player1.Front, player1.Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        player1.Up = glm::normalize(glm::cross(player1.Right, player1.Front));


        if (numberOfPlayers == 2.0f)
        {
            //move
            if (player2.playerCurrentSpeed < -530)
                player2.playerCurrentSpeed = -530;

            if (player2.playerCurrentSpeed > 530)
                player2.playerCurrentSpeed = 530;

            player2.playerRotation += player2.playerCurrentTurnSpeed / 100;
            double distance2 = player2.playerCurrentSpeed / 100;// *deltaTime;
            double dx2 = (double)(distance2 * sin(player2.playerRotation + M_PI / 2));
            double dz2 = (double)(distance2 * cos(player2.playerRotation + M_PI / 2));
            player2.playerPosition += glm::vec3(dx2, 0.0f, dz2);

        // calculate the new Front vector
            glm::vec3 front2;
            front2.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
            front2.y = sin(glm::radians(0.0f));
            front2.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
            player2.Front = glm::normalize(front2);
            // also re-calculate the Right and Up vector
            player2.Right = glm::normalize(glm::cross(player2.Front, player2.Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            player2.Up = glm::normalize(glm::cross(player2.Right, player2.Front));
        }

        // render
        // ------
        gltBeginDraw();

        // Draw Model
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gltDrawText2D(text1, 10.0f, 10.0f, 1000000.0f); // x=0.0, y=0.0, scale=1.0

        gltDrawText2DAligned(text1,
            (GLfloat)(SCR_WIDTH / 2),
            (GLfloat)(SCR_HEIGHT / 2),
            3.0f,
            GLT_CENTER, GLT_CENTER);


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

    {
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

/////////////////////////////////////////camera 2 ///////////////////////////////////////////////

            ourShader.use();
            ourShader.setVec3("viewPos", camera2.Position);
            ourShader.setFloat("material.shininess", 32.0f);

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

            ourShader.setVec3("spotLight.position", camera2.Position.x, camera2.Position.y, camera2.Position.z);

            ourShader.setVec3("spotLight.direction", camera2.Front.x, camera2.Front.y, camera2.Front.z);

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
    }


        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/ (float)SCR_HEIGHT * numberOfPlayers, 1.0f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrixAtPlayer(player1.playerPosition);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);

        glm::mat4 victorianHouse = glm::mat4(1.0f);
        victorianHouse = glm::translate(victorianHouse, glm::vec3(0.0f, -1.75f, 0.0f));
        victorianHouse = glm::scale(victorianHouse, glm::vec3(0.05f, 0.05f, 0.05f));
        ourShader.setMat4("model", victorianHouse);
        victorianHouseModel.Draw(ourShader);

    //road was here

        //Background Loader
        backgroundShader.use();
        backgroundShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderSphere();

       //draw model using normal shader
        float tiltRotation = 1;


       /* ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPosition);
        if (playerCurrentTurnSpeed > 0.0f)//left
        {
            camera.changeCamYaw(50);


            //glPushMatrix(); // Save ModelView
           //  model = glm::rotate(model, tiltRotation, glm::vec3(0.0f, 0.0f, 1.0f));
           // glPopMatrix(); // Restore ModelView

        }
        else  if (playerCurrentTurnSpeed < 0.0f)//right
        {
           // model = glm::translate(model, -playerPosition);
               // model = glm::rotate(model, -tiltRotation* playerRotation, glm::vec3(1.0f, 0.0f, 1.0f));
            //    model = glm::translate(model, playerPosition);
        }     
            //    model = glm::translate(model, playerPosition); 
        model = glm::rotate(model, playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        RedCar.Draw(ourShader); */

        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, player1.playerPosition);
        model = glm::rotate(model, player1.playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        RedCar.Draw(ourShader);

        if (numberOfPlayers == 2.0f)
        {
            glm::mat4 model2 = glm::mat4(1.0f);
            model2 = glm::translate(model2, player2.playerPosition);
            model2 = glm::rotate(model2, player2.playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model2 = glm::scale(model2, glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", model2);
            RedCar2.Draw(ourShader);
        }

        glm::mat4 newtrack = glm::mat4(1.0f);
        newtrack = glm::translate(newtrack, glm::vec3(100.0f, 11.0f, -400.0f));
        newtrack = glm::scale(newtrack, glm::vec3(3.5f, 3.5f, 3.5f));
        ourShader.setMat4("model", newtrack);
        track.Draw(ourShader);

        if (numberOfPlayers == 2.0f)
        {
                // setup your right view projection:
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT/2);
               /* glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH, SCR_HEIGHT);
                glOrtho(0, SCR_WIDTH, SCR_WIDTH, 0, -1, 1);
                glMatrixMode(GL_MODELVIEW);*/


            camera2.Position = glm::vec3(player2.playerPosition.x + 10.0f,
                player2.playerPosition.y + 5.0f,
                player2.playerPosition.z);


            camera2.move(player2.playerRotation, player2.playerPosition, player2.pressed);

///////////////////////////////////////player1
//move
            if (player1.playerCurrentSpeed < -530)
                player1.playerCurrentSpeed = -530;

            if (player1.playerCurrentSpeed > 530)
                player1.playerCurrentSpeed = 530;




            player1.playerRotation += player1.playerCurrentTurnSpeed / 100;
            double distance = player1.playerCurrentSpeed / 100;// *deltaTime;
            double dx = (double)(distance * sin(player1.playerRotation + M_PI / 2));
            double dz = (double)(distance * cos(player1.playerRotation + M_PI / 2));
            player1.playerPosition += glm::vec3(dx, 0.0f, dz);

            // calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
            front.y = sin(glm::radians(0.0f));
            front.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
            player1.Front = glm::normalize(front);
            // also re-calculate the Right and Up vector
            player1.Right = glm::normalize(glm::cross(player1.Front, player1.Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            player1.Up = glm::normalize(glm::cross(player1.Right, player1.Front));

/////////////////////////player2
//move
                if (player2.playerCurrentSpeed < -530)
                    player2.playerCurrentSpeed = -530;

                if (player2.playerCurrentSpeed > 530)
                    player2.playerCurrentSpeed = 530;

                player2.playerRotation += player2.playerCurrentTurnSpeed / 100;
                double distance2 = player2.playerCurrentSpeed / 100;// *deltaTime;
                double dx2 = (double)(distance2 * sin(player2.playerRotation + M_PI / 2));
                double dz2 = (double)(distance2 * cos(player2.playerRotation + M_PI / 2));
                player2.playerPosition += glm::vec3(dx2, 0.0f, dz2);

            // calculate the new Front vector
                glm::vec3 front2;
                front2.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
                front2.y = sin(glm::radians(0.0f));
                front2.z = sin(glm::radians(-90.0f)) * cos(glm::radians((0.0f)));
                player2.Front = glm::normalize(front);
                // also re-calculate the Right and Up vector
                player2.Right = glm::normalize(glm::cross(player2.Front, player2.Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
                player2.Up = glm::normalize(glm::cross(player2.Right, player2.Front));
            
            // render
            // ------
            //gltBeginDraw();

            // Draw Model
          //  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
          //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          //  gltDrawText2D(text1, 10.0f, 10.0f, 1000000.0f); // x=0.0, y=0.0, scale=1.0

          //  gltDrawText2DAligned(text1,
          //      (GLfloat)(SCR_WIDTH / 2),
          //      (GLfloat)(SCR_HEIGHT / 2),
          //      3.0f,
          //      GLT_CENTER, GLT_CENTER);


            ourShader.use();
            ourShader.setVec3("viewPos", camera2.Position);
            ourShader.setFloat("material.shininess", 32.0f);

            {
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

                    ourShader.setVec3("spotLight.position", camera2.Position.x, camera2.Position.y, camera2.Position.z);

                    ourShader.setVec3("spotLight.direction", camera2.Front.x, camera2.Front.y, camera2.Front.z);

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

                    ourShader.setVec3("spotLight.position", camera2.Position.x, camera2.Position.y, camera2.Position.z);

                    ourShader.setVec3("spotLight.direction", camera2.Front.x, camera2.Front.y, camera2.Front.z);

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

                    ourShader.setVec3("spotLight.position", camera2.Position.x, camera2.Position.y, camera2.Position.z);

                    ourShader.setVec3("spotLight.direction", camera2.Front.x, camera2.Front.y, camera2.Front.z);

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

                    ourShader.setVec3("spotLight.position", camera2.Position.x, camera2.Position.y, camera2.Position.z);

                    ourShader.setVec3("spotLight.direction", camera2.Front.x, camera2.Front.y, camera2.Front.z);

                    ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);

                    ourShader.setVec3("spotLight.diffuse", 0.0f, 1.0f, 0.0f);

                    ourShader.setVec3("spotLight.specular", 0.0f, 1.0f, 0.0f);

                    ourShader.setFloat("spotLight.constant", 1.0f);

                    ourShader.setFloat("spotLight.linear", 0.07);

                    ourShader.setFloat("spotLight.quadratic", 0.017);

                    ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.0f)));

                    ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(10.0f)));
                }
            }


            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT * numberOfPlayers, 1.0f, 10000.0f);
            glm::mat4 view = camera2.GetViewMatrixAtPlayer(player2.playerPosition);
             


            //  glm::mat4 projection2 = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / numberOfPlayers / (float)SCR_HEIGHT, 1.0f, 10000.0f);
           //   glm::mat4 view2 = camera2.GetViewMatrixAtPlayer(player2.playerPosition);


            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);




            // bind diffuse map
            glActiveTexture(GL_TEXTURE0);

            glm::mat4 victorianHouse = glm::mat4(1.0f);
            victorianHouse = glm::translate(victorianHouse, glm::vec3(0.0f, -1.75f, 0.0f));
            victorianHouse = glm::scale(victorianHouse, glm::vec3(0.05f, 0.05f, 0.05f));
            ourShader.setMat4("model", victorianHouse);
            victorianHouseModel.Draw(ourShader);

            //road was here

                //Background Loader
            backgroundShader.use();

            backgroundShader.setMat4("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

            renderSphere();

            float tiltRotation = 1;

            //Draw models
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, player1.playerPosition);
            model = glm::rotate(model, player1.playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            RedCar.Draw(ourShader);

            glm::mat4 model2 = glm::mat4(1.0f);
            model2 = glm::translate(model2, player2.playerPosition);
            model2 = glm::rotate(model2, player2.playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model2 = glm::scale(model2, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model2);
            RedCar2.Draw(ourShader);

            glm::mat4 newtrack = glm::mat4(1.0f);
            newtrack = glm::translate(newtrack, glm::vec3(100.0f, 11.0f, -400.0f));
            newtrack = glm::scale(newtrack, glm::vec3(3.5f, 3.5f, 3.5f));
            ourShader.setMat4("model", newtrack);
            track.Draw(ourShader);
        }
        //draw hud
        //----------------------------------
        glEnable2D();
        //do 2d stuff 
        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0);
        glVertex2f(0.0, 0.0);
        glVertex2f(10.0, 0.0);
        glVertex2f(10.0, 10.0);
        glVertex2f(0.0, 10.0);
        glEnd();

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);
        // render container
    //    ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);

        gltDrawText2D(text1, 10.0f, 10.0f, 1000000.0f); // x=0.0, y=0.0, scale=1.0

        gltDrawText2DAligned(text1,
            (GLfloat)(SCR_WIDTH / 2),
            (GLfloat)(SCR_HEIGHT / 2),
            3.0f,
            GLT_CENTER, GLT_CENTER);
        // end 2d
        glDisable2D();
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //text
    gltDeleteText(text1);
    gltDeleteText(text2);

    gltTerminate();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void glEnable2D()
{
    int vPort[4] = { 0, 0, SCR_WIDTH, SCR_HEIGHT };

    glGetIntegerv(GL_VIEWPORT, vPort);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

  //  glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
    glOrtho(0.0, SCR_WIDTH, 0.0, SCR_HEIGHT, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    //OPTIONAL STUFF
    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void glDisable2D()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    //escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //forward / backward player1
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player1.playerCurrentSpeed<=0.0 )//forward
    {
        player1.playerCurrentSpeed -= player1.playerAcceleration* deltaTime ;
        //playerCurrentSpeed = -MovementSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && player1.playerCurrentSpeed >= 0.0)//backward
    {
        player1.playerCurrentSpeed += player1.playerAcceleration * deltaTime  * 0.7 ;
        //playerCurrentSpeed = MovementSpeed * 0.7f;
    }     
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player1.playerCurrentSpeed >= 0.0)//forward while still going backward
    {
        player1.playerCurrentSpeed -= player1.playerAcceleration * deltaTime;
        //playerCurrentSpeed = -MovementSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && player1.playerCurrentSpeed <= 0.0)//backward while still going forward
    {
        player1.playerCurrentSpeed += player1.playerAcceleration * deltaTime * 0.7;
        //playerCurrentSpeed = MovementSpeed * 0.7f;
    }
    else if (player1.playerCurrentSpeed > 0.0)//drag force when w key is relesed
    {
        player1.playerCurrentSpeed -= 18000/ player1.playerCurrentSpeed  * deltaTime;
        if (player1.playerCurrentSpeed < 0.0)
        {
            player1.playerCurrentSpeed = 0.0;
        }
    }
    else if (player1.playerCurrentSpeed < 0.0)//drag force when s key is relesed
    {
        player1.playerCurrentSpeed -= 18000/ player1.playerCurrentSpeed * deltaTime;
        if (player1.playerCurrentSpeed > 0.0)
        {
            player1.playerCurrentSpeed = 0.0;
       }
    }



    //left / right player 1
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
        player1.playerCurrentSpeed<0.0 && player1.playerCurrentTurnSpeed>=0.0)//turn left when going forward
    {
        player1.playerCurrentTurnSpeed += -0.005 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed > player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = player1.playerMaxTurnSpeed;
    }      
    else    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed <= 0.0)//turn right when going forward
    {
        player1.playerCurrentTurnSpeed += 0.005 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed < -player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = -player1.playerMaxTurnSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        player1.playerCurrentSpeed > 0.0&& player1.playerCurrentTurnSpeed >= 0.0)//turn left when going backward
    {
        player1.playerCurrentTurnSpeed += -0.008 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed > player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = player1.playerMaxTurnSpeed;
    }
    else    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS 
        && player1.playerCurrentSpeed > 0.0 && player1.playerCurrentTurnSpeed <= 0.0)//turn right when backward
    {
        player1.playerCurrentTurnSpeed += 0.008 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed < -player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = -player1.playerMaxTurnSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS 
        && player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed < 0.0)//turn left when still turning righ and going forward 
    {
        player1.playerCurrentTurnSpeed = 15 * deltaTime;
    }
    else    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS 
        && player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed > 0.0)//turn right when still turning left and going forward
    {
        player1.playerCurrentTurnSpeed -= 15 * deltaTime;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        player1.playerCurrentSpeed > 0.0 && player1.playerCurrentTurnSpeed < 0.0)//turn left when still turning righ and going backward
    {
        player1.playerCurrentTurnSpeed = 16 * deltaTime;
    }
    else    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
        player1.playerCurrentSpeed > 0.0 && player1.playerCurrentTurnSpeed > 0.0)//turn right when still turning left and going backward
    {
        player1.playerCurrentTurnSpeed -= 16 * deltaTime;
    }
    else if (player1.playerCurrentTurnSpeed > 0.0)//keep turning for lil bit after relesing key
    {
        player1.playerCurrentTurnSpeed -= 9* deltaTime;
        if (player1.playerCurrentTurnSpeed < 0.0)
        {
            player1.playerCurrentTurnSpeed = 0.0;
            player1.playerAcceleration = 100.0;
            player1.pressed = false;
        }
    }
    else if (player1.playerCurrentTurnSpeed < 0.0)//keep turning for lil bit after relesing key
    {
        player1.playerCurrentTurnSpeed += 9* deltaTime;
        if (player1.playerCurrentTurnSpeed > 0.0)
        {
            player1.playerCurrentTurnSpeed = 0.0;
            player1.playerAcceleration = 100.0;
            player1.pressed = false;
        }
    }


    //brake player1
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player1.playerCurrentSpeed > 0.0)
    {
      // SoundEngine->play2D("resources/sounds/bleep.mp3", false);
        player1.playerCurrentSpeed -= 80000/ player1.playerCurrentSpeed *deltaTime;

        if (player1.playerCurrentTurnSpeed < 0.0 || player1.playerCurrentTurnSpeed > 0.0)
        {
          //  playerCurrentTurnSpeed -= playerCurrentTurnSpeed / 2 * deltaTime;
        }
        if (player1.playerCurrentSpeed < 0.0)
        {
            player1.playerCurrentSpeed = 0.0;
        }
    }
     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player1.playerCurrentSpeed < 0.0)
     {
         player1.playerCurrentSpeed -= 80000 / player1.playerCurrentSpeed * deltaTime;
        if (player1.playerCurrentTurnSpeed < 0.0 || player1.playerCurrentTurnSpeed > 0.0)
        {
           // playerCurrentTurnSpeed -= playerCurrentTurnSpeed / 2 * deltaTime;
        }
        if (player1.playerCurrentSpeed > 0.0)
        {
            player1.playerCurrentSpeed = 0.0;
        }
     }

////////////////////////player 2/////////////////////////////////////

     //forward / backward player2
     if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && player2.playerCurrentSpeed <= 0.0)//forward
     {
         player2.playerCurrentSpeed -= player2.playerAcceleration * deltaTime;
         //playerCurrentSpeed = -MovementSpeed;
     }
     else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && player2.playerCurrentSpeed >= 0.0)//backward
     {
         player2.playerCurrentSpeed += player2.playerAcceleration * deltaTime * 0.7;
         //playerCurrentSpeed = MovementSpeed * 0.7f;
     }
     else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && player2.playerCurrentSpeed >= 0.0)//forward while still going backward
     {
         player2.playerCurrentSpeed -= player2.playerAcceleration * deltaTime;
         //playerCurrentSpeed = -MovementSpeed;
     }
     else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && player2.playerCurrentSpeed <= 0.0)//backward while still going forward
     {
         player2.playerCurrentSpeed += player2.playerAcceleration * deltaTime * 0.7;
         //playerCurrentSpeed = MovementSpeed * 0.7f;
     }
     else if (player2.playerCurrentSpeed > 0.0)//drag force when w key is relesed
     {
         player2.playerCurrentSpeed -= 18000 / player2.playerCurrentSpeed * deltaTime;
         if (player2.playerCurrentSpeed < 0.0)
         {
             player2.playerCurrentSpeed = 0.0;
         }
     }
     else if (player2.playerCurrentSpeed < 0.0)//drag force when s key is relesed
     {
         player2.playerCurrentSpeed -= 18000 / player2.playerCurrentSpeed * deltaTime;
         if (player2.playerCurrentSpeed > 0.0)
         {
             player2.playerCurrentSpeed = 0.0;
         }
     }



     //left / right player 2
     if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS &&
         player2.playerCurrentSpeed < 0.0 && player2.playerCurrentTurnSpeed >= 0.0)//turn left when going forward
     {
         player2.playerCurrentTurnSpeed += -0.005 * player2.playerCurrentSpeed * deltaTime;
         player2.playerCurrentSpeed *= 0.9999;
         player2.playerAcceleration = 30.00;
         player2.pressed = true;
         if (player2.playerCurrentTurnSpeed > player2.playerMaxTurnSpeed)
             player2.playerCurrentTurnSpeed = player2.playerMaxTurnSpeed;
     }
     else    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
         player2.playerCurrentSpeed < 0.0 && player2.playerCurrentTurnSpeed <= 0.0)//turn right when going forward
     {
         player2.playerCurrentTurnSpeed += 0.005 * player2.playerCurrentSpeed * deltaTime;
         player2.playerCurrentSpeed *= 0.9999;
         player2.playerAcceleration = 30.00;
         player2.pressed = true;
         if (player2.playerCurrentTurnSpeed < -player2.playerMaxTurnSpeed)
             player2.playerCurrentTurnSpeed = -player2.playerMaxTurnSpeed;
     }
     else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
         player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed >= 0.0)//turn left when going backward
     {
         player2.playerCurrentTurnSpeed += -0.008 * player2.playerCurrentSpeed * deltaTime;
         player2.playerCurrentSpeed *= 0.9999;
         player2.playerAcceleration = 30.00;
         player2.pressed = true;
         if (player2.playerCurrentTurnSpeed > player2.playerMaxTurnSpeed)
             player2.playerCurrentTurnSpeed = player2.playerMaxTurnSpeed;
     }
     else    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS
         && player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed <= 0.0)//turn right when backward
     {
         player2.playerCurrentTurnSpeed += 0.008 * player2.playerCurrentSpeed * deltaTime;
         player2.playerCurrentSpeed *= 0.9999;
         player2.playerAcceleration = 30.00;
         player2.pressed = true;
         if (player2.playerCurrentTurnSpeed < -player2.playerMaxTurnSpeed)
             player2.playerCurrentTurnSpeed = -player2.playerMaxTurnSpeed;
     }
     else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS
         && player2.playerCurrentSpeed < 0.0 && player2.playerCurrentTurnSpeed < 0.0)//turn left when still turning righ and going forward 
     {
         player2.playerCurrentTurnSpeed = 15 * deltaTime;
     }
     else    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
         && player2.playerCurrentSpeed < 0.0 && player2.playerCurrentTurnSpeed > 0.0)//turn right when still turning left and going forward
     {
         player2.playerCurrentTurnSpeed -= 15 * deltaTime;
     }
     else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
         player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed < 0.0)//turn left when still turning righ and going backward
     {
         player2.playerCurrentTurnSpeed = 16 * deltaTime;
     }
     else    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS &&
         player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed > 0.0)//turn right when still turning left and going backward
     {
         player2.playerCurrentTurnSpeed -= 16 * deltaTime;
     }
     else if (player2.playerCurrentTurnSpeed > 0.0)//keep turning for lil bit after relesing key
     {
         player2.playerCurrentTurnSpeed -= 9 * deltaTime;
         if (player2.playerCurrentTurnSpeed < 0.0)
         {
             player2.playerCurrentTurnSpeed = 0.0;
             player2.playerAcceleration = 100.0;
             player2.pressed = false;
         }
     }
     else if (player2.playerCurrentTurnSpeed < 0.0)//keep turning for lil bit after relesing key
     {
         player2.playerCurrentTurnSpeed += 9 * deltaTime;
         if (player2.playerCurrentTurnSpeed > 0.0)
         {
             player2.playerCurrentTurnSpeed = 0.0;
             player2.playerAcceleration = 100.0;
             player2.pressed = false;
         }
     }


     //brake player2
     if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && player2.playerCurrentSpeed > 0.0)
     {
         // SoundEngine->play2D("resources/sounds/bleep.mp3", false);
         player2.playerCurrentSpeed -= 80000 / player2.playerCurrentSpeed * deltaTime;

         if (player2.playerCurrentTurnSpeed < 0.0 || player2.playerCurrentTurnSpeed > 0.0)
         {
             //  playerCurrentTurnSpeed -= playerCurrentTurnSpeed / 2 * deltaTime;
         }
         if (player2.playerCurrentSpeed < 0.0)
         {
             player2.playerCurrentSpeed = 0.0;
         }
     }
     if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && player2.playerCurrentSpeed < 0.0)
     {
         player2.playerCurrentSpeed -= 80000 / player2.playerCurrentSpeed * deltaTime;
         if (player2.playerCurrentTurnSpeed < 0.0 || player2.playerCurrentTurnSpeed > 0.0)
         {
             // playerCurrentTurnSpeed -= playerCurrentTurnSpeed / 2 * deltaTime;
         }
         if (player2.playerCurrentSpeed > 0.0)
         {
             player2.playerCurrentSpeed = 0.0;
         }
     }

    
     //drift Does not work yet. Idea for futer;
     // lower playerTurnSpeed growth and maxTurnSpeed so drifting seems more useful,
     // figure out nice camera movement (disconect player and camera ?) 
     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
         player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed >= 0.0 &&
         glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)//turn left when going forward
     {
         player1.playerCurrentTurnSpeed *= 70 * deltaTime;
         player1.playerMaxTurnSpeed = 4.0f;
     }
     else    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
         player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed <= 0.0 &&
         glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) //turn right when going forward
     {
         player1.playerCurrentTurnSpeed *= 70 * deltaTime;
         player1.playerMaxTurnSpeed = 4.0f;
     }
     else
         player1.playerMaxTurnSpeed = 3.2f;


     //Press P for two players mode
     if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
     {
         PisPressed = true;
     }
     if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && PisPressed == true)
     {
         PisPressed = false;
         if (numberOfPlayers == 1.0f)
             numberOfPlayers = 2.0f;
         else
             numberOfPlayers = 1.0f;
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
/*
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
}*/
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

