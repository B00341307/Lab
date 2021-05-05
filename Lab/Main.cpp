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
#include "AI.h"

#include <iostream>
#include <vector>
#include <math.h>


#include <ctime> //std::clock()

#include <stdio.h>

#include <irrKlang.h>//audio
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include "Player.h"

// sound set up
ISoundEngine* engine2 = createIrrKlangDevice();
bool engine2on = false;

ISoundEngine* engine2P2 = createIrrKlangDevice();
bool engine2onP2 = false;

// sound set up
ISoundEngine* ringSound = createIrrKlangDevice();
bool ringSoundon = false;

ISoundEngine* newLapsound = createIrrKlangDevice();

ISoundEngine* Winnersound = createIrrKlangDevice();

float numberOfPlayers = 1.0f;
bool PisPressed = false;


bool CollisonDetection(glm::vec3 one, glm::vec3 two, float p1Rotation, float p2Rotation);
glm::vec3 CollisonResult1, CollisonResult2;

void glDisable2D();
void glEnable2D();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void renderSphere();
int key = 1;

double tempdp;

// settings                     2k:     wide 2k:    fullhd:
const unsigned int SCR_WIDTH = /*2560;*/    /*3440;*/     1920;
const unsigned int SCR_HEIGHT = /*1440;*/   /*1440;*/      1080;

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



class Ring
{
public:
    glm::vec3 ringPosition;
    float ringRoatation;


    //For Ring Explosion effect
    double explosionLenght = -1.0;
    bool explosionis = false;
    double explosionTime;

};



Player player1;
Player player2;
 

//Game Loop
bool gameStarted = false;
bool gameFinnished = false;

int main()
{
    player1.playerPosition = glm::vec3(5.0f, 0.0f, -53.0f);
    player2.playerPosition = glm::vec3(-15.0f, 0.0f, -33.0f);
    // sound set up
    ISoundEngine* engine = createIrrKlangDevice();

    // play background sound, looped
    engine->setSoundVolume(0.4f);
    engine->play2D("resources/sounds/breakout.mp3", true);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Race The Game", NULL, NULL);
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
  //  Shader shader("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");
 //  Shader shader2("Shaders/9.3.default.vs", "Shaders/9.3.default.fs");
  //  Shader normalShader("Shaders/9.3.normal_visualization.vs", "Shaders/9.3.normal_visualization.fs", "Shaders/9.3.normal_visualization.gs");
  //  Shader shader_explosion("Shaders/9.2.geometry_shader.vs", "Shaders/9.2.geometry_shader.fs", "Shaders/9.2.geometry_shader.gs");
    Shader ourShader("1.model_loading.v2.vs", "1.model_loading.v2.fs");
    Shader equirectangularToCubemapShader("2.2.2.cubemap.vs", "2.2.2.equirectangular_to_cubemap.fs");
    Shader backgroundShader("2.2.2.background.vs", "2.2.2.background.fs");
    Shader toonShader("Shaders/Toon.vs", "Toon.fs");
    Shader shader_explosion("Shaders/Geometry_shader.vs", "Shaders/Geometry_shader.fs", "Shaders/Geometry_shader.gs");

    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    backgroundShader.use();
    backgroundShader.setInt("environmentMap", 0);

    toonShader.use();
    toonShader.setInt("toon", 0);


    // load models-
    Model victorianHouseModel("resources/objects/Victorian House/Victorian House 2 8 edit 2.obj");
    Model countryRoadModel("resources/objects/country road/terreno02.obj");

    stbi_set_flip_vertically_on_load(true);
    Model RedCar("resources/objects/RedCarColours/RedCar.obj");
    Model RedCar2("resources/objects/BlueCar/RedCar.obj");
    Model track("resources/objects/track/track.obj");
    Model RingObj("resources/objects/ring/ringobj.obj");

    Model NewTrack("resources/NewTrack2/track2.obj");
    Model Walls("resources/Walls/track2walls.obj");
    Model InnerWalls("resources/InnerWalls/track2walls2.obj");

    //Hud
    Model startHud("resources/HUD/start/startHud.obj");
    Model winner("resources/HUD/Winner/startHud.obj");
    Model looser("resources/HUD/Looser/startHud.obj");

    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT);
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


    double time = 1000;
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


    Ring Ring1; Ring Ring2; Ring Ring3; Ring Ring4; Ring Ring5; Ring Ring6;  Ring Ring7;  Ring Ring8;
    std::vector<glm::vec3*> Checkpoints;
    glm::vec3 V1(-9, 0, -9);
    glm::vec3 V2(-12, 0, 221);
    glm::vec3 V3(473, 0, 228);
    glm::vec3 V4(589, 0, 207);
    glm::vec3 V5(607, 0, -46);
    glm::vec3 V53(474, 0, -25);
    glm::vec3 V6(177, 0, -41);
    glm::vec3 V7(537, 0, -219);
    glm::vec3 V8(591, 0, -457);
    glm::vec3 V9(3, 0, -425);

    Checkpoints.push_back(&V1);
    Checkpoints.push_back(&V2);
    Checkpoints.push_back(&Ring1.ringPosition);
    Checkpoints.push_back(&Ring2.ringPosition);
    Checkpoints.push_back(&V3);
    Checkpoints.push_back(&V4);
    Checkpoints.push_back(&V5);
    Checkpoints.push_back(&V53);
    Checkpoints.push_back(&Ring4.ringPosition);
    Checkpoints.push_back(&V6);
    Checkpoints.push_back(&Ring7.ringPosition);
    Checkpoints.push_back(&V7);
    Checkpoints.push_back(&V8);
    Checkpoints.push_back(&Ring3.ringPosition);
    Checkpoints.push_back(&Ring6.ringPosition);
    Checkpoints.push_back(&Ring5.ringPosition);
    Checkpoints.push_back(&V9);
    Checkpoints.push_back(&Ring8.ringPosition);
    AI playerAi_(Checkpoints, glm::vec3(0, 0, 0), &player1.playerPosition, &RedCar, &ourShader);
    //----------------------------------------------------------------------------------
    // render loop----------------------------------------------------------------------
    // ---------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        CollisonResult1 = glm::vec3(0.f, 0.f, 0.f);
        CollisonResult2 = CollisonResult1;

        // input
        // -----     
        processInput(window);

        if (numberOfPlayers == 2.0f)
        {
            //top screen for 2player mode
            glViewport(0, SCR_HEIGHT / 2, SCR_WIDTH, SCR_HEIGHT / 2);
        }
        else
        {
            //normal screen
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        }

        camera.Position = glm::vec3(player1.playerPosition.x + 10.0f,
            player1.playerPosition.y + 5.0f,
            player1.playerPosition.z);

        camera.move(player1.playerRotation, player1.playerPosition, player1.pressed);

        player1.PlayerMove();
        player1.PlayerNewFront();


        if (numberOfPlayers == 2.0f)
        {
            player2.PlayerMove();
            player2.PlayerNewFront();
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        //Kamil's lightning modes
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


        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT * numberOfPlayers, 1.0f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrixAtPlayer(player1.playerPosition);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);

       /* glm::mat4 victorianHouse = glm::mat4(1.0f);
        victorianHouse = glm::translate(victorianHouse, glm::vec3(0.0f, -1.75f, 0.0f));
        victorianHouse = glm::scale(victorianHouse, glm::vec3(0.05f, 0.05f, 0.05f));
        ourShader.setMat4("model", victorianHouse);
        victorianHouseModel.Draw(ourShader);*/

        //road was here

        //Background Loader
            backgroundShader.use();
            backgroundShader.setMat4("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            renderSphere();
       

        //draw model using normal shader
        float tiltRotation = 1;

        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, player1.playerPosition);
        model = glm::rotate(model, player1.playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        RedCar.Draw(ourShader);

        playerAi_.BeginDrawTranslate();
    

        glm::mat4 newtrack = glm::mat4(1.0f);
        newtrack = glm::translate(newtrack, glm::vec3(0.0f, 0.0f, 0.0f));
        newtrack = glm::scale(newtrack, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", newtrack);
        NewTrack.Draw(ourShader);


        glm::mat4 walls = glm::mat4(1.0f);
        walls = glm::translate(walls, glm::vec3(0.0f, -0.3f, -595.0f));
        walls = glm::scale(walls, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", walls);
        Walls.Draw(ourShader);
         
        glm::mat4 walls2 = glm::mat4(1.0f);
        walls2 = glm::translate(walls2, glm::vec3(0.0f, 0.0f, -660.0f));
        walls2 = glm::rotate(walls2, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        walls2 = glm::scale(walls2, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", walls2);
        InnerWalls.Draw(ourShader);

        //Hud

        if (numberOfPlayers == 2.0f)
        {
            glm::mat4 model2 = glm::mat4(1.0f);
            model2 = glm::translate(model2, player2.playerPosition);
            model2 = glm::rotate(model2, player2.playerRotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model2 = glm::scale(model2, glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", model2);
            RedCar2.Draw(ourShader);
      
        }
        // rings
        {
            Ring1.ringPosition = glm::vec3(179.0f, -0.3f, 161.5f);
            glm::mat4 ring = glm::mat4(1.0f);
            ring = glm::translate(ring, Ring1.ringPosition);
            ring = glm::scale(ring, glm::vec3(2.0f, 2.0f, 2.0f));

            //chack collision with ring      
            if (player1.playerPosition.x - Ring1.ringPosition.x < 6.0f && player1.playerPosition.x - Ring1.ringPosition.x > -6.0f &&
                player1.playerPosition.z - Ring1.ringPosition.z < 6.0f && player1.playerPosition.z - Ring1.ringPosition.z > -6.0f &&
                Ring1.explosionis == false)
            {
                Ring1.explosionTime = std::clock();
                Ring1.explosionis = true;
                ringSound->play2D("resources/sounds/coin.wav");
                player1.PlayerGotBoost();
            }

            if (Ring1.explosionis && std::clock() < Ring1.explosionTime + 1000)
            {
                shader_explosion.use();
                shader_explosion.setMat4("projection", projection);
                shader_explosion.setMat4("view", view);
                shader_explosion.setMat4("model", ring);
                shader_explosion.setFloat("time", Ring1.explosionLenght);
                Ring1.explosionLenght += deltaTime;
                RingObj.Draw(shader_explosion);
            }
            else
            {
                Ring1.explosionis = false;
                Ring1.explosionLenght = -1.0f;
                ourShader.use();
                ourShader.setMat4("projection", projection);
                ourShader.setMat4("view", view);
                ourShader.setMat4("model", ring);
                RingObj.Draw(ourShader);
            }
        

        //ring2
        Ring2.ringPosition = glm::vec3(250.0f, -0.3f, 161.5f);
        glm::mat4 ring2 = glm::mat4(1.0f);
        ring2 = glm::translate(ring2, Ring2.ringPosition);
        ring2 = glm::scale(ring2, glm::vec3(2.0f, 2.0f, 2.0f));

        //chack collision with ring      
        if (player1.playerPosition.x - Ring2.ringPosition.x < 6.0f && player1.playerPosition.x - Ring2.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring2.ringPosition.z < 6.0f && player1.playerPosition.z - Ring2.ringPosition.z > -6.0f &&
            Ring2.explosionis == false)
        {
            Ring2.explosionTime = std::clock();
            Ring2.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }

        if (Ring2.explosionis && std::clock() < Ring2.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring2);
            shader_explosion.setFloat("time", Ring2.explosionLenght);
            Ring2.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);         
        }
        else
        {
            Ring2.explosionis = false;
            Ring2.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring2);
            RingObj.Draw(ourShader);
        }


        //ring3
        Ring3.ringPosition = glm::vec3(536.0f, -0.3f, -464.2f);
        glm::mat4 ring3 = glm::mat4(1.0f);
        ring3 = glm::translate(ring3, Ring3.ringPosition);
        ring3 = glm::scale(ring3, glm::vec3(2.0f, 2.0f, 2.0f));

        //chack collision with ring      
        if (player1.playerPosition.x - Ring3.ringPosition.x < 6.0f && player1.playerPosition.x - Ring3.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring3.ringPosition.z < 6.0f && player1.playerPosition.z - Ring3.ringPosition.z > -6.0f &&
            Ring3.explosionis == false)
        {
            Ring3.explosionTime = std::clock();
            Ring3.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }

        if (Ring3.explosionis && std::clock() < Ring3.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring3);
            shader_explosion.setFloat("time", Ring3.explosionLenght);
            Ring3.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
            Ring3.explosionis = false;
            Ring3.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring3);
            RingObj.Draw(ourShader);
        }
        
        //ring4
        Ring4.ringPosition = glm::vec3(337.34f, -0.3f, 33.2f);
        glm::mat4 ring4 = glm::mat4(1.0f);
        ring4 = glm::translate(ring4, Ring4.ringPosition);
        ring4 = glm::scale(ring4, glm::vec3(2.0f, 2.0f, 2.0f));

       //chack collision with ring      
        if (player1.playerPosition.x - Ring4.ringPosition.x < 6.0f && player1.playerPosition.x - Ring4.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring4.ringPosition.z < 6.0f && player1.playerPosition.z - Ring4.ringPosition.z > -6.0f &&
            Ring4.explosionis == false)
        {
            Ring4.explosionTime = std::clock();
            Ring4.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }
        if (Ring4.explosionis && std::clock() < Ring4.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring4);
            shader_explosion.setFloat("time", Ring4.explosionLenght);
            Ring4.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
            Ring4.explosionis = false;
            Ring4.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring4);
            RingObj.Draw(ourShader);
        }

        //ring5
        Ring5.ringPosition = glm::vec3(458.0f, -0.3f, -464.2f);
        glm::mat4 ring5 = glm::mat4(1.0f);
        ring5 = glm::translate(ring5, Ring5.ringPosition);
        ring5 = glm::scale(ring5, glm::vec3(2.0f, 2.0f, 2.0f));

        //chack collision with ring      
        if (player1.playerPosition.x - Ring5.ringPosition.x < 6.0f && player1.playerPosition.x - Ring5.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring5.ringPosition.z < 6.0f && player1.playerPosition.z - Ring5.ringPosition.z > -6.0f &&
            Ring5.explosionis == false)
        {
            Ring5.explosionTime = std::clock();
            Ring5.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }

        if (Ring5.explosionis && std::clock() < Ring5.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring5);
            shader_explosion.setFloat("time", Ring5.explosionLenght);
            Ring5.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
            Ring5.explosionis = false;
            Ring5.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring5);
            RingObj.Draw(ourShader);
        }
        
        //ring6
        Ring6.ringPosition = glm::vec3(496.34f, -0.3f, -464.2f);
        glm::mat4 ring6 = glm::mat4(1.0f);
        ring6 = glm::translate(ring6, Ring6.ringPosition);
        ring6 = glm::scale(ring6, glm::vec3(2.0f, 2.0f, 2.0f));

        //chack collision with ring      
        if (player1.playerPosition.x - Ring6.ringPosition.x < 6.0f && player1.playerPosition.x - Ring6.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring6.ringPosition.z < 6.0f && player1.playerPosition.z - Ring6.ringPosition.z > -6.0f &&
            Ring6.explosionis == false)
        {
            Ring6.explosionTime = std::clock();
            Ring6.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }
        if (Ring6.explosionis && std::clock() < Ring6.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring6);
            shader_explosion.setFloat("time", Ring6.explosionLenght);
            Ring6.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
            Ring6.explosionis = false;
            Ring6.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring6);
            RingObj.Draw(ourShader);
        }

        //ring7
        Ring7.ringPosition = glm::vec3(276.34f, -0.3f, -158.1f);
        glm::mat4 ring7 = glm::mat4(1.0f);
        ring7 = glm::translate(ring7, Ring7.ringPosition);
        ring7 = glm::scale(ring7, glm::vec3(2.0f, 2.0f, 2.0f));

        //chack collision with ring      
        if (player1.playerPosition.x - Ring7.ringPosition.x < 6.0f && player1.playerPosition.x - Ring7.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring7.ringPosition.z < 6.0f && player1.playerPosition.z - Ring7.ringPosition.z > -6.0f &&
            Ring7.explosionis == false)
        {
            Ring7.explosionTime = std::clock();
            Ring7.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }
        if (Ring7.explosionis && std::clock() < Ring7.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring7);
            shader_explosion.setFloat("time", Ring7.explosionLenght);
            Ring7.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
            Ring7.explosionis = false;
            Ring7.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring7);
            RingObj.Draw(ourShader);
        }

        //ring8
        Ring8.ringPosition = glm::vec3(40.2f, -0.3f,-208.0f);
        glm::mat4 ring8 = glm::mat4(1.0f);

        ring8 = glm::translate(ring8, Ring8.ringPosition);
        ring8 = glm::rotate(ring8, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        ring8 = glm::scale(ring8, glm::vec3(2.0f, 2.0f, 2.0f));

        //chack collision with ring      
        if (player1.playerPosition.x - Ring8.ringPosition.x < 6.0f && player1.playerPosition.x - Ring8.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ring8.ringPosition.z < 6.0f && player1.playerPosition.z - Ring8.ringPosition.z > -6.0f &&
            Ring8.explosionis == false)
        {
            Ring8.explosionTime = std::clock();
            Ring8.explosionis = true;

            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
           // ringSoundon = true;

            // engine2->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3", true);
   // engine2->drop(); // delete engine
      //  ISoundEngine* engine4 = createIrrKlangDevice();
      //  engine4->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3", true);
        }
        if (Ring8.explosionis && std::clock() < Ring8.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ring8);
            shader_explosion.setFloat("time", Ring8.explosionLenght);
            Ring8.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
               // ringSound->stopAllSounds();
              //  ringSoundon = false;
            
            Ring8.explosionis = false;
            Ring8.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ring8);
            RingObj.Draw(ourShader);
        }

        if (player1.BoostTimer > 0 && std::clock() > player1.BoostTimer + 1800)
        {
            player1.playerMaxSpeed = 150.0f;
            player1.PlayerBoost = 0;
            player1.BoostTimer = 0;
        }

        /*   //ring4
        Ring Ringg;
        Ringg.ringPosition = glm::vec3(337.34f, -0.3f, 33.2f);
        glm::mat4 ringg = glm::mat4(1.0f);
        ringg = glm::translate(ringg, Ringg.ringPosition);
        ringg = glm::scale(ringg, glm::vec3(2.0f, 2.0f, 2.0f));

       //chack collision with ring      
        if (player1.playerPosition.x - Ringg.ringPosition.x < 6.0f && player1.playerPosition.x - Ringg.ringPosition.x > -6.0f &&
            player1.playerPosition.z - Ringg.ringPosition.z < 6.0f && player1.playerPosition.z - Ringg.ringPosition.z > -6.0f &&
            Ringg.explosionis == false)
        {
            Ringg.explosionTime = std::clock();
        if (Ringg.explosionis && std::clock() < Ringg.explosionTime + 1000)
        {
            shader_explosion.use();
            shader_explosion.setMat4("projection", projection);
            shader_explosion.setMat4("view", view);
            shader_explosion.setMat4("model", ringg);
            shader_explosion.setFloat("time", Ringg.explosionLenght);
            Ringg.explosionLenght += deltaTime;
            RingObj.Draw(shader_explosion);
        }
        else
        {
            Ringg.explosionis = false;
            Ringg.explosionLenght = -1.0f;
            ourShader.use();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", ringg);
            RingObj.Draw(ourShader);
        }*/
        }
        //check collsion with gate 1     
        if (player1.playerPosition.x > -98.0f && player1.playerPosition.x < 90.0f &&
            player1.playerPosition.z - 13.0f < 12.0f && player1.playerPosition.z - 13.0f > -12.0f &&
            player1.points == 0 ||
            player1.playerPosition.x > -98.0f && player1.playerPosition.x < 90.0f &&
            player1.playerPosition.z - 13.0f < 12.0f && player1.playerPosition.z - 13.0f > -12.0f &&
            player1.points == 3)
        {
            player1.points++;
        }

        //check collsion with gate 2     
        if (player1.playerPosition.x - 285.0f < 12.0f && player1.playerPosition.x - 285.0f > -12.0f &&
            player1.playerPosition.z > -246.0f && player1.playerPosition.z < -82.0f &&
            player1.points == 1)
        {
            player1.points++;
        }

        //check collsion with gate 3     
        if (player1.playerPosition.x - 320.0f < 12.0f && player1.playerPosition.x - 320.0f > -12.0f &&
            player1.playerPosition.z > -482.0f && player1.playerPosition.z < -390.0f &&
            player1.points == 2)
        {
            player1.points++;
        }


        //Count Laps
        if (player1.points == 4)
        {
            player1.points = 1;
            player1.Laps++;
            newLapsound->play2D("resources/sounds/337049__shinephoenixstormcrow__320655-rhodesmas-level-up-01.mp3");
        }

        if (player1.Laps == 2)
        {
            gameFinnished = true;
            player1.playerCurrentSpeed = 0.0f;
            player2.playerCurrentSpeed = 0.0f;
            Winnersound->play2D("resources/sounds/applause3.wav");
            player1.Laps++;
        }
        //check collsion with gate 1     
     /* if (player1.playerPosition.x > -98.0f && player1.playerPosition.x < -6.0f &&
            player1.playerPosition.z - 13.0f < 12.0f && player1.playerPosition.z - 13.0f > -12.0f &&
            Ring1.explosionis == false)
        {
            Ring1.explosionTime = std::clock();
            Ring1.explosionis = true;
            ringSound->play2D("resources/sounds/coin.wav");
            player1.PlayerGotBoost();
        }*/  


        //draw hud
        //----------------------------------    
        glEnable2D();
        glm::mat4 hud1 = glm::mat4(1.0f);
        hud1 = glm::translate(hud1, player1.playerPosition);
        hud1 = glm::rotate(hud1, player1.playerRotation - 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        hud1 = glm::scale(hud1, glm::vec3(1.5f, 1.5f, 1.5f));
        ourShader.setMat4("model", hud1);
        if (gameStarted == false)
        {
            startHud.Draw(ourShader);
        }
        if (gameFinnished && gameStarted)
        {
            if (player1.Laps == 3)
                winner.Draw(ourShader);
            else
                looser.Draw(ourShader);
        }

        glDisable2D();

    


        if (numberOfPlayers == 2.0f)
        {
           
            //bottom view projection:
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT / 2);

            camera2.Position = glm::vec3(player2.playerPosition.x + 10.0f,
                player2.playerPosition.y + 5.0f,
                player2.playerPosition.z);

            camera2.move(player2.playerRotation, player2.playerPosition, player2.pressed);
          
            // render          

            ourShader.use();
            ourShader.setVec3("viewPos", camera2.Position);
            ourShader.setFloat("material.shininess", 32.0f);

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT * numberOfPlayers, 1.0f, 10000.0f);
            glm::mat4 view = camera2.GetViewMatrixAtPlayer(player2.playerPosition);


            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
 
            // bind diffuse map
            glActiveTexture(GL_TEXTURE0);

          /*  glm::mat4 victorianHouse = glm::mat4(1.0f);
            victorianHouse = glm::translate(victorianHouse, glm::vec3(0.0f, -1.75f, 0.0f));
            victorianHouse = glm::scale(victorianHouse, glm::vec3(0.05f, 0.05f, 0.05f));
            ourShader.setMat4("model", victorianHouse);
            victorianHouseModel.Draw(ourShader); */

            //road was here
             //Background Loader
            backgroundShader.use();
            backgroundShader.setMat4("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            renderSphere();

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

            glm::mat4 newtrack2 = glm::mat4(1.0f);
            newtrack2 = glm::translate(newtrack2, glm::vec3(0.0f, 0.0f, 0.0f));
            newtrack2 = glm::scale(newtrack2, glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", newtrack2);
            NewTrack.Draw(ourShader);

            glm::mat4 walls2 = glm::mat4(1.0f);
            walls2 = glm::translate(walls2, glm::vec3(0.0f, -0.3f, -595.0f));
            walls2 = glm::scale(walls2, glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", walls2);
            Walls.Draw(ourShader);

            glm::mat4 innerWalls2 = glm::mat4(1.0f);
            innerWalls2 = glm::translate(innerWalls2, glm::vec3(0.0f, 0.0f, -660.0f));
            innerWalls2 = glm::rotate(innerWalls2, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            innerWalls2 = glm::scale(innerWalls2, glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", innerWalls2);
            InnerWalls.Draw(ourShader);


            if (CollisonDetection(player1.playerPosition, player2.playerPosition,
                player1.playerRotation, player2.playerRotation))
            {
                player1.playerPosition += CollisonResult1;
                player2.playerPosition += CollisonResult2;

                player1.playerCurrentSpeed *= 0.5f;
                player2.playerCurrentSpeed *= 0.5f;
            }

            // rings
            {
                Ring1.ringPosition = glm::vec3(179.0f, -0.3f, 161.5f);
                glm::mat4 ring = glm::mat4(1.0f);
                ring = glm::translate(ring, Ring1.ringPosition);
                ring = glm::scale(ring, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring1.ringPosition.x < 6.0f && player2.playerPosition.x - Ring1.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring1.ringPosition.z < 6.0f && player2.playerPosition.z - Ring1.ringPosition.z > -6.0f &&
                    Ring1.explosionis == false)
                {
                    Ring1.explosionTime = std::clock();
                    Ring1.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }

                if (Ring1.explosionis && std::clock() < Ring1.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring);
                    shader_explosion.setFloat("time", Ring1.explosionLenght);
                    Ring1.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring1.explosionis = false;
                    Ring1.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring);
                    RingObj.Draw(ourShader);
                }


                //ring2
                Ring2.ringPosition = glm::vec3(250.0f, -0.3f, 161.5f);
                glm::mat4 ring2 = glm::mat4(1.0f);
                ring2 = glm::translate(ring2, Ring2.ringPosition);
                ring2 = glm::scale(ring2, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring2.ringPosition.x < 6.0f && player2.playerPosition.x - Ring2.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring2.ringPosition.z < 6.0f && player2.playerPosition.z - Ring2.ringPosition.z > -6.0f &&
                    Ring2.explosionis == false)
                {
                    Ring2.explosionTime = std::clock();
                    Ring2.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }

                if (Ring2.explosionis && std::clock() < Ring2.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring2);
                    shader_explosion.setFloat("time", Ring2.explosionLenght);
                    Ring2.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring2.explosionis = false;
                    Ring2.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring2);
                    RingObj.Draw(ourShader);
                }


                //ring3
                Ring3.ringPosition = glm::vec3(536.0f, -0.3f, -464.2f);
                glm::mat4 ring3 = glm::mat4(1.0f);
                ring3 = glm::translate(ring3, Ring3.ringPosition);
                ring3 = glm::scale(ring3, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring3.ringPosition.x < 6.0f && player2.playerPosition.x - Ring3.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring3.ringPosition.z < 6.0f && player2.playerPosition.z - Ring3.ringPosition.z > -6.0f &&
                    Ring3.explosionis == false)
                {
                    Ring3.explosionTime = std::clock();
                    Ring3.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }

                if (Ring3.explosionis && std::clock() < Ring3.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring3);
                    shader_explosion.setFloat("time", Ring3.explosionLenght);
                    Ring3.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring3.explosionis = false;
                    Ring3.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring3);
                    RingObj.Draw(ourShader);
                }

                //ring4
                Ring4.ringPosition = glm::vec3(337.34f, -0.3f, 33.2f);
                glm::mat4 ring4 = glm::mat4(1.0f);
                ring4 = glm::translate(ring4, Ring4.ringPosition);
                ring4 = glm::scale(ring4, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring4.ringPosition.x < 6.0f && player2.playerPosition.x - Ring4.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring4.ringPosition.z < 6.0f && player2.playerPosition.z - Ring4.ringPosition.z > -6.0f &&
                    Ring4.explosionis == false)
                {
                    Ring4.explosionTime = std::clock();
                    Ring4.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }
                if (Ring4.explosionis && std::clock() < Ring4.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring4);
                    shader_explosion.setFloat("time", Ring4.explosionLenght);
                    Ring4.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring4.explosionis = false;
                    Ring4.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring4);
                    RingObj.Draw(ourShader);
                }

                //ring5
                Ring5.ringPosition = glm::vec3(458.0f, -0.3f, -464.2f);
                glm::mat4 ring5 = glm::mat4(1.0f);
                ring5 = glm::translate(ring5, Ring5.ringPosition);
                ring5 = glm::scale(ring5, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring5.ringPosition.x < 6.0f && player2.playerPosition.x - Ring5.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring5.ringPosition.z < 6.0f && player2.playerPosition.z - Ring5.ringPosition.z > -6.0f &&
                    Ring5.explosionis == false)
                {
                    Ring5.explosionTime = std::clock();
                    Ring5.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }

                if (Ring5.explosionis && std::clock() < Ring5.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring5);
                    shader_explosion.setFloat("time", Ring5.explosionLenght);
                    Ring5.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring5.explosionis = false;
                    Ring5.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring5);
                    RingObj.Draw(ourShader);
                }

                //ring6
                Ring6.ringPosition = glm::vec3(496.34f, -0.3f, -464.2f);
                glm::mat4 ring6 = glm::mat4(1.0f);
                ring6 = glm::translate(ring6, Ring6.ringPosition);
                ring6 = glm::scale(ring6, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring6.ringPosition.x < 6.0f && player2.playerPosition.x - Ring6.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring6.ringPosition.z < 6.0f && player2.playerPosition.z - Ring6.ringPosition.z > -6.0f &&
                    Ring6.explosionis == false)
                {
                    Ring6.explosionTime = std::clock();
                    Ring6.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }
                if (Ring6.explosionis && std::clock() < Ring6.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring6);
                    shader_explosion.setFloat("time", Ring6.explosionLenght);
                    Ring6.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring6.explosionis = false;
                    Ring6.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring6);
                    RingObj.Draw(ourShader);
                }

                //ring7
                Ring7.ringPosition = glm::vec3(276.34f, -0.3f, -158.1f);
                glm::mat4 ring7 = glm::mat4(1.0f);
                ring7 = glm::translate(ring7, Ring7.ringPosition);
                ring7 = glm::scale(ring7, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring7.ringPosition.x < 6.0f && player2.playerPosition.x - Ring7.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring7.ringPosition.z < 6.0f && player2.playerPosition.z - Ring7.ringPosition.z > -6.0f &&
                    Ring7.explosionis == false)
                {
                    Ring7.explosionTime = std::clock();
                    Ring7.explosionis = true;
                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                }
                if (Ring7.explosionis && std::clock() < Ring7.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring7);
                    shader_explosion.setFloat("time", Ring7.explosionLenght);
                    Ring7.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    Ring7.explosionis = false;
                    Ring7.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring7);
                    RingObj.Draw(ourShader);
                }

                //ring8
                Ring8.ringPosition = glm::vec3(40.2f, -0.3f, -208.0f);
                glm::mat4 ring8 = glm::mat4(1.0f);

                ring8 = glm::translate(ring8, Ring8.ringPosition);
                ring8 = glm::rotate(ring8, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
                ring8 = glm::scale(ring8, glm::vec3(2.0f, 2.0f, 2.0f));

                //chack collision with ring      
                if (player2.playerPosition.x - Ring8.ringPosition.x < 6.0f && player2.playerPosition.x - Ring8.ringPosition.x > -6.0f &&
                    player2.playerPosition.z - Ring8.ringPosition.z < 6.0f && player2.playerPosition.z - Ring8.ringPosition.z > -6.0f &&
                    Ring8.explosionis == false)
                {
                    Ring8.explosionTime = std::clock();
                    Ring8.explosionis = true;

                    ringSound->play2D("resources/sounds/coin.wav");
                    player2.PlayerGotBoost();
                    // ringSoundon = true;

                     // engine2->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3", true);
            // engine2->drop(); // delete engine
               //  ISoundEngine* engine4 = createIrrKlangDevice();
               //  engine4->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3", true);
                }
                if (Ring8.explosionis && std::clock() < Ring8.explosionTime + 1000)
                {
                    shader_explosion.use();
                    shader_explosion.setMat4("projection", projection);
                    shader_explosion.setMat4("view", view);
                    shader_explosion.setMat4("model", ring8);
                    shader_explosion.setFloat("time", Ring8.explosionLenght);
                    Ring8.explosionLenght += deltaTime;
                    RingObj.Draw(shader_explosion);
                }
                else
                {
                    // ringSound->stopAllSounds();
                   //  ringSoundon = false;

                    Ring8.explosionis = false;
                    Ring8.explosionLenght = -1.0f;
                    ourShader.use();
                    ourShader.setMat4("projection", projection);
                    ourShader.setMat4("view", view);
                    ourShader.setMat4("model", ring8);
                    RingObj.Draw(ourShader);
                }

                if (player2.BoostTimer > 0 && std::clock() > player2.BoostTimer + 1800)
                {
                    player2.playerMaxSpeed = 150.0f;
                    player2.PlayerBoost = 0;
                    player2.BoostTimer = 0;
                }
            }

            //check collsion with gate 1     
            if (player2.playerPosition.x > -98.0f && player2.playerPosition.x < 90.0f &&
                player2.playerPosition.z - 13.0f < 12.0f && player2.playerPosition.z - 13.0f > -12.0f &&
                player2.points == 0 ||
                player2.playerPosition.x > -98.0f && player2.playerPosition.x < 90.0f &&
                player2.playerPosition.z - 13.0f < 12.0f && player2.playerPosition.z - 13.0f > -12.0f &&
                player2.points == 3)
            {
                player2.points++;
            }

            //check collsion with gate 2     
            if (player2.playerPosition.x - 285.0f < 12.0f && player2.playerPosition.x - 285.0f > -12.0f &&
                player2.playerPosition.z > -246.0f && player2.playerPosition.z < -82.0f &&
                player2.points == 1)
            {
                player2.points++;
            }

            //check collsion with gate 3     
            if (player2.playerPosition.x - 320.0f < 12.0f && player2.playerPosition.x - 320.0f > -12.0f &&
                player2.playerPosition.z > -482.0f && player2.playerPosition.z < -390.0f &&
                player2.points == 2)
            {
                player2.points++;
            }

            //Count Laps
            if (player2.points == 4)
            {
                player2.points = 1;
                player2.Laps++;
                newLapsound->play2D("resources/sounds/337049__shinephoenixstormcrow__320655-rhodesmas-level-up-01.mp3");
            }

            if (player2.Laps == 2)
            {
                gameFinnished = true;
                player1.playerCurrentSpeed = 0.0f;
                player2.playerCurrentSpeed = 0.0f;
                Winnersound->play2D("resources/sounds/applause3.wav");
                player2.Laps++;
            }

            //draw hud
            //----------------------------------    
            glEnable2D();
            glm::mat4 hud1 = glm::mat4(1.0f);
            hud1 = glm::translate(hud1, player2.playerPosition);
            hud1 = glm::rotate(hud1, player2.playerRotation - 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
            hud1 = glm::scale(hud1, glm::vec3(1.5f, 1.5f, 1.5f));
            ourShader.setMat4("model", hud1);
            if (gameStarted == false)
            {
                startHud.Draw(ourShader);
            }
            if (gameFinnished && gameStarted)
            {
                if (player2.Laps == 3)
                    winner.Draw(ourShader);
                else
                    looser.Draw(ourShader);
            }
            glDisable2D();

        }

        if (gameStarted == true)
        {
            playerAi_.Update(deltaTime);
        }

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// car to car
bool CollisonDetection(glm::vec3 one, glm::vec3 two, float p1Rotation, float p2Rotation)
{
    //check just x and z as the game is flat

    bool collisionX = false;
    bool collisionZ = false;

    float distance1 = 6.5f;
    float distance2 = 1.5f;
    glm::vec3 bottom1, bottom2, leftBottom1, leftBottom2, rightBottom1, rightBottom2;
    glm::vec3 leftTop1, leftTop2, rightTop1, rightTop2;

    glm::vec3 corner;
    bool IsColl1 = false;

    double dx2 = (double)(distance1 * sin(p1Rotation + M_PI / 2));
    double dz2 = (double)(distance1 * cos(p1Rotation + M_PI / 2));
    bottom1 = one + glm::vec3(dx2, 0.0f, dz2);

    dx2 = (double)(distance1 * sin(p2Rotation + M_PI / 2));
    dz2 = (double)(distance1 * cos(p2Rotation + M_PI / 2));

    bottom2 = two + glm::vec3(dx2, 0.0f, dz2);


    dx2 = (double)(distance2 * sin(p1Rotation));
    dz2 = (double)(distance2 * cos(p1Rotation));

    leftBottom1 = bottom1 + glm::vec3(dx2, 0.0f, dz2);
    rightBottom1 = bottom1 - glm::vec3(dx2, 0.0f, dz2);

    leftTop1 = one + glm::vec3(dx2, 0.0f, dz2);
    rightTop1 = one - glm::vec3(dx2, 0.0f, dz2);

    dx2 = (double)(distance2 * sin(p2Rotation));
    dz2 = (double)(distance2 * cos(p2Rotation));

    leftBottom2 = bottom2 + glm::vec3(dx2, 0.0f, dz2);
    rightBottom2 = bottom2 - glm::vec3(dx2, 0.0f, dz2);

    leftTop2 = two + glm::vec3(dx2, 0.0f, dz2);
    rightTop2 = two - glm::vec3(dx2, 0.0f, dz2);


    if (leftTop1.x >= leftBottom2.x && leftTop1.x <= rightBottom2.x ||
        leftTop1.x <= leftBottom2.x && leftTop1.x >= rightBottom2.x ||
        leftTop1.x >= leftBottom2.x && leftTop1.x <= rightTop2.x ||
        leftTop1.x <= leftBottom2.x && leftTop1.x >= rightTop2.x ||
        leftTop1.x >= leftTop2.x && leftTop1.x <= rightTop2.x ||
        leftTop1.x <= leftTop2.x && leftTop1.x >= rightTop2.x ||
        leftTop1.x >= rightTop2.x && leftTop1.x <= rightBottom2.x ||
        leftTop1.x <= rightTop2.x && leftTop1.x >= rightBottom2.x)
    {
        IsColl1 = true;
        corner = leftTop1;
    }
    else if (rightTop1.x >= leftBottom2.x && rightTop1.x <= rightBottom2.x ||
        rightTop1.x <= leftBottom2.x && rightTop1.x >= rightBottom2.x ||
        rightTop1.x >= leftBottom2.x && rightTop1.x <= rightTop2.x ||
        rightTop1.x <= leftBottom2.x && rightTop1.x >= rightTop2.x ||
        rightTop1.x >= leftTop2.x && rightTop1.x <= rightTop2.x ||
        rightTop1.x <= leftTop2.x && rightTop1.x >= rightTop2.x ||
        rightTop1.x >= rightTop2.x && rightTop1.x <= rightBottom2.x ||
        rightTop1.x <= rightTop2.x && rightTop1.x >= rightBottom2.x)
    {
        IsColl1 = true;
        corner = rightTop1;
    }
    else if (
        rightBottom1.x >= leftBottom2.x && rightBottom1.x <= rightBottom2.x ||
        rightBottom1.x <= leftBottom2.x && rightBottom1.x >= rightBottom2.x ||
        rightBottom1.x >= leftBottom2.x && rightBottom1.x <= rightTop2.x ||
        rightBottom1.x <= leftBottom2.x && rightBottom1.x >= rightTop2.x ||
        rightBottom1.x >= leftTop2.x && rightBottom1.x <= rightTop2.x ||
        rightBottom1.x <= leftTop2.x && rightBottom1.x >= rightTop2.x ||
        rightBottom1.x >= rightTop2.x && rightBottom1.x <= rightBottom2.x ||
        rightBottom1.x <= rightTop2.x && rightBottom1.x >= rightBottom2.x)
    {
        IsColl1 = true;
        corner = rightBottom1;
    }
    else if (
        leftBottom1.x >= leftBottom2.x && leftBottom1.x <= rightBottom2.x ||
        leftBottom1.x <= leftBottom2.x && leftBottom1.x >= rightBottom2.x ||
        leftBottom1.x >= leftBottom2.x && leftBottom1.x <= rightTop2.x ||
        leftBottom1.x <= leftBottom2.x && leftBottom1.x >= rightTop2.x ||
        leftBottom1.x >= leftTop2.x && leftBottom1.x <= rightTop2.x ||
        leftBottom1.x <= leftTop2.x && leftBottom1.x >= rightTop2.x ||
        leftBottom1.x >= rightTop2.x && leftBottom1.x <= rightBottom2.x ||
        leftBottom1.x <= rightTop2.x && leftBottom1.x >= rightBottom2.x)
    {
        IsColl1 = true;
        corner = leftBottom1;
    }

    if (IsColl1)
    {
        glm::vec3 Tempvar;
        float tempx, tempz;
        if (leftTop1.z >= leftBottom2.z && leftTop1.z <= rightBottom2.z ||
            leftTop1.z <= leftBottom2.z && leftTop1.z >= rightBottom2.z ||
            leftTop1.z >= leftBottom2.z && leftTop1.z <= rightTop2.z ||
            leftTop1.z <= leftBottom2.z && leftTop1.z >= rightTop2.z ||
            leftTop1.z >= leftTop2.z && leftTop1.z <= rightTop2.z ||
            leftTop1.z <= leftTop2.z && leftTop1.z >= rightTop2.z ||
            leftTop1.z >= rightTop2.z && leftTop1.z <= rightBottom2.z ||
            leftTop1.z <= rightTop2.z && leftTop1.z >= rightBottom2.z ||

            rightTop1.z >= leftBottom2.z && rightTop1.z <= rightBottom2.z ||
            rightTop1.z <= leftBottom2.z && rightTop1.z >= rightBottom2.z ||
            rightTop1.z >= leftBottom2.z && rightTop1.z <= rightTop2.z ||
            rightTop1.z <= leftBottom2.z && rightTop1.z >= rightTop2.z ||
            rightTop1.z >= leftTop2.z && rightTop1.z <= rightTop2.z ||
            rightTop1.z <= leftTop2.z && rightTop1.z >= rightTop2.z ||
            rightTop1.z >= rightTop2.z && rightTop1.z <= rightBottom2.z ||
            rightTop1.z <= rightTop2.z && rightTop1.z >= rightBottom2.z ||

            rightBottom1.z >= leftBottom2.z && rightBottom1.z <= rightBottom2.z ||
            rightBottom1.z <= leftBottom2.z && rightBottom1.z >= rightBottom2.z ||
            rightBottom1.z >= leftBottom2.z && rightBottom1.z <= rightTop2.z ||
            rightBottom1.z <= leftBottom2.z && rightBottom1.z >= rightTop2.z ||
            rightBottom1.z >= leftTop2.z && rightBottom1.z <= rightTop2.z ||
            rightBottom1.z <= leftTop2.z && rightBottom1.z >= rightTop2.z ||
            rightBottom1.z >= rightTop2.z && rightBottom1.z <= rightBottom2.z ||
            rightBottom1.z <= rightTop2.z && rightBottom1.z >= rightBottom2.z ||

            leftBottom1.z >= leftBottom2.z && leftBottom1.z <= rightBottom2.z ||
            leftBottom1.z <= leftBottom2.z && leftBottom1.z >= rightBottom2.z ||
            leftBottom1.z >= leftBottom2.z && leftBottom1.z <= rightTop2.z ||
            leftBottom1.z <= leftBottom2.z && leftBottom1.z >= rightTop2.z ||
            leftBottom1.z >= leftTop2.z && leftBottom1.z <= rightTop2.z ||
            leftBottom1.z <= leftTop2.z && leftBottom1.z >= rightTop2.z ||
            leftBottom1.z >= rightTop2.z && leftBottom1.z <= rightBottom2.z ||
            leftBottom1.z <= rightTop2.z && leftBottom1.z >= rightBottom2.z)
        {


            Tempvar = leftBottom2;
            if (sqrt(pow((leftBottom2.x - corner.x), 2) + pow((leftBottom2.z - corner.z), 2)) >
                sqrt(pow((rightBottom2.x - corner.x), 2) + pow((rightBottom2.z - corner.z), 2)))
            {
                Tempvar = rightBottom2;
            }
            else if (sqrt(pow((Tempvar.x - corner.x), 2) + pow((Tempvar.z - corner.z), 2)) >
                sqrt(pow((rightTop2.x - corner.x), 2) + pow((rightTop2.z - corner.z), 2)))
            {
                Tempvar = rightTop2;
            }
            else if (sqrt(pow((Tempvar.x - corner.x), 2) + pow((Tempvar.z - corner.z), 2)) >
                sqrt(pow((leftTop2.x - corner.x), 2) + pow((leftTop2.z - corner.z), 2)))
            {
                Tempvar = leftTop2;
            }

            if (Tempvar.x > corner.x)
            {
                tempx = Tempvar.x - corner.x;
            }
            else
                tempx = corner.x - Tempvar.x;

            if (Tempvar.z > corner.z)
            {
                tempz = Tempvar.z - corner.z;
            }
            else
                tempz = corner.z - Tempvar.z;

            if (abs(tempx) > abs(tempz))
            {
                if (corner.x < Tempvar.x)
                {
                    CollisonResult1.x = (Tempvar.x - corner.x) / 2;
                    CollisonResult2.x = (Tempvar.x - corner.x) / -2;
                }
                else
                {
                    CollisonResult1.x = (corner.x - Tempvar.x) / 2;
                    CollisonResult2.x = (corner.x - Tempvar.x) / -2;
                }
            }
            else
            {
                if (corner.z < Tempvar.z)
                {
                    CollisonResult1.z = (Tempvar.z - corner.z) / 2;
                    CollisonResult2.z = (Tempvar.z - corner.z) / -2;
                }
                else
                {
                    CollisonResult1.z = (corner.z - Tempvar.z) / 2;
                    CollisonResult2.z = (corner.z - Tempvar.z) / -2;
                }
            }
        }
        else
            IsColl1 = false;
    }
    return IsColl1;
}




void glEnable2D()
{
    int vPort[4] = { 0, 0, SCR_WIDTH, SCR_HEIGHT };

    glGetIntegerv(GL_VIEWPORT, vPort);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, vPort[2], vPort[3], 0, -1, 1);
    //glOrtho(0.0, SCR_WIDTH, 0.0, SCR_HEIGHT, 0.0, 1.0);
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

void processInput(GLFWwindow* window)
{
    if (gameStarted == false)
    {
        //escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

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
        //start
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            gameStarted = true;
        }
    }
    else if (gameStarted && gameFinnished)
    {
        //escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

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
        //start
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            gameFinnished = false;
            player1.playerPosition = glm::vec3(5.0f, 0.0f, -53.0f);
            player2.playerPosition = glm::vec3(-15.0f, 0.0f, -33.0f);
            player1.points = 0;
            player1.Laps = 0;
            player2.points = 0;
            player2.Laps = 0;
            player1.playerRotation = 1.5f;
            player2.playerRotation = 1.5f;
        }

    }
    else
    {
        //leave this comment i need it
       // engine2->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3", true);
        // engine2->drop(); // delete engine
       // ISoundEngine* engine3 = createIrrKlangDevice();
       // engine3->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3", true);
        // engine2->drop(); // delete engine





    //print location and rotation
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        cout << player1.playerPosition.x << " " << player1.playerPosition.z << " " << player1.playerRotation << "\n";
    }

    //escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //forward / backward player1
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player1.playerCurrentSpeed <= 0.0)//forward
    {
        player1.playerCurrentSpeed -= player1.playerAcceleration * deltaTime;

        //playerCurrentSpeed = -MovementSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && player1.playerCurrentSpeed >= 0.0)//backward
    {
        player1.playerCurrentSpeed += player1.playerAcceleration * deltaTime * 0.7;
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
        player1.playerCurrentSpeed -= 18000 / player1.playerCurrentSpeed * deltaTime;
        if (player1.playerCurrentSpeed < 0.0)
        {
            player1.playerCurrentSpeed = 0.0;
        }
    }
    else if (player1.playerCurrentSpeed < 0.0)//drag force when s key is relesed
    {
        player1.playerCurrentSpeed -= 18000 / player1.playerCurrentSpeed * deltaTime;
        if (player1.playerCurrentSpeed > 0.0)
        {
            player1.playerCurrentSpeed = 0.0;
        }
    }

    //Engine sound p1
    if (player1.playerCurrentSpeed != 0.0 && engine2on == false)
    {
        engine2->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3");
        engine2on = true;
    }

    if (player1.playerCurrentSpeed == 0.0 && engine2on == true)
    {
        engine2->stopAllSounds();
        //engine2->drop();
        engine2on = false;
    }

    //left / right player 1
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
        player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed >= 0.0)//turn left when going forward
    {
        player1.playerCurrentTurnSpeed += -0.05 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed > player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = player1.playerMaxTurnSpeed;
    }
    else    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        player1.playerCurrentSpeed < 0.0 && player1.playerCurrentTurnSpeed <= 0.0)//turn right when going forward
    {
        player1.playerCurrentTurnSpeed += 0.05 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed < -player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = -player1.playerMaxTurnSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
        player1.playerCurrentSpeed > 0.0 && player1.playerCurrentTurnSpeed >= 0.0)//turn left when going backward
    {
        player1.playerCurrentTurnSpeed += -0.08 * player1.playerCurrentSpeed * deltaTime;
        player1.playerCurrentSpeed *= 0.9999;
        player1.playerAcceleration = 30.00;
        player1.pressed = true;
        if (player1.playerCurrentTurnSpeed > player1.playerMaxTurnSpeed)
            player1.playerCurrentTurnSpeed = player1.playerMaxTurnSpeed;
    }
    else    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
        && player1.playerCurrentSpeed > 0.0 && player1.playerCurrentTurnSpeed <= 0.0)//turn right when backward
    {
        player1.playerCurrentTurnSpeed += 0.08 * player1.playerCurrentSpeed * deltaTime;
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
        player1.playerCurrentTurnSpeed = 15 * deltaTime;
    }
    else    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
        player1.playerCurrentSpeed > 0.0 && player1.playerCurrentTurnSpeed > 0.0)//turn right when still turning left and going backward
    {
        player1.playerCurrentTurnSpeed -= 15 * deltaTime;
    }
    else if (player1.playerCurrentTurnSpeed > 0.0)//keep turning for lil bit after relesing key
    {
        player1.playerCurrentTurnSpeed -= 9 * deltaTime;
        if (player1.playerCurrentTurnSpeed < 0.0)
        {
            player1.playerCurrentTurnSpeed = 0.0;
            player1.playerAcceleration = 100.0;
            player1.pressed = false;
        }
    }
    else if (player1.playerCurrentTurnSpeed < 0.0)//keep turning for lil bit after relesing key
    {
        player1.playerCurrentTurnSpeed += 9 * deltaTime;
        if (player1.playerCurrentTurnSpeed > 0.0)
        {
            player1.playerCurrentTurnSpeed = 0.0;
            player1.playerAcceleration = 100.0;
            player1.pressed = false;
        }
    }


    //brake player1
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player1.playerCurrentSpeed > 0.0)
    {
        player1.playerCurrentSpeed -= 8000 / player1.playerCurrentSpeed * deltaTime;

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
       player1.playerCurrentSpeed -= 8000 / player1.playerCurrentSpeed * deltaTime;
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

    //Engine sound p2
    if (player2.playerCurrentSpeed != 0.0 && engine2onP2 == false)
    {
        engine2P2->play2D("resources/sounds/Sports-Car-Driving-Med-www.fesliyanstudios.com.mp3");
        engine2onP2 = true;
    }

    if (player2.playerCurrentSpeed == 0.0 && engine2onP2 == true)
    {
        engine2P2->stopAllSounds();
        //engine2->drop();
        engine2onP2 = false;
    }



    //left / right player 2
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        player2.playerCurrentSpeed < 0.0 && player2.playerCurrentTurnSpeed >= 0.0)//turn left when going forward
    {
        player2.playerCurrentTurnSpeed += -0.05 * player2.playerCurrentSpeed * deltaTime;
        player2.playerCurrentSpeed *= 0.9999;
        player2.playerAcceleration = 30.00;
        player2.pressed = true;
        if (player2.playerCurrentTurnSpeed > player2.playerMaxTurnSpeed)
            player2.playerCurrentTurnSpeed = player2.playerMaxTurnSpeed;
    }
    else    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        player2.playerCurrentSpeed < 0.0 && player2.playerCurrentTurnSpeed <= 0.0)//turn right when going forward
    {
        player2.playerCurrentTurnSpeed += 0.05 * player2.playerCurrentSpeed * deltaTime;
        player2.playerCurrentSpeed *= 0.9999;
        player2.playerAcceleration = 30.00;
        player2.pressed = true;
        if (player2.playerCurrentTurnSpeed < -player2.playerMaxTurnSpeed)
            player2.playerCurrentTurnSpeed = -player2.playerMaxTurnSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed >= 0.0)//turn left when going backward
    {
        player2.playerCurrentTurnSpeed += -0.08 * player2.playerCurrentSpeed * deltaTime;
        player2.playerCurrentSpeed *= 0.9999;
        player2.playerAcceleration = 30.00;
        player2.pressed = true;
        if (player2.playerCurrentTurnSpeed > player2.playerMaxTurnSpeed)
            player2.playerCurrentTurnSpeed = player2.playerMaxTurnSpeed;
    }
    else    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS
        && player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed <= 0.0)//turn right when backward
    {
        player2.playerCurrentTurnSpeed += 0.08 * player2.playerCurrentSpeed * deltaTime;
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
        player2.playerCurrentTurnSpeed = 15 * deltaTime;
    }
    else    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        player2.playerCurrentSpeed > 0.0 && player2.playerCurrentTurnSpeed > 0.0)//turn right when still turning left and going backward
    {
        player2.playerCurrentTurnSpeed -= 15 * deltaTime;
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