#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "light.hpp"

// Create camera object
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));

// Timers
float currentTime = 0.0f;
float lastTime = 0.0f;
float deltaTime = 0.0f;

// Data structures


struct Object
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
    float ka = 0.2f;
    float kd = 0.7f;
    float ks = 1.0f;
    float Ns = 20.0f;
    std::string name;
};


int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow( 1024, 768, "Normal Mapping", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Use back face culling
    glEnable(GL_CULL_FACE);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Compile shader programs
    GLuint shaderID = LoadShaders("normalMapVertexShader.vert", "normalMapFragmentShader.frag");
    
    // Load models
    Model teapot("../objects/teapot.obj");
    Model lightModel("../objects/sphere.obj");
    Model floor("../objects/flat_plane.obj");
    Model wall("../objects/wall.obj");

    // Add textures to models
    teapot.addTexture("../objects/blue_diffuse.bmp", "diffuse");
    teapot.addTexture("../objects/diamond_normal.png", "normal");
    floor.addTexture("../objects/stones_diffuse.png", "diffuse");
    floor.addTexture("../objects/stones_normal.png", "normal");
    floor.addTexture("../objects/stones_specular.png", "specular");

    wall.addTexture("../objects/bricks_diffuse.png", "diffuse");
    wall.addTexture("../objects/bricks_normal.png", "normal");
    wall.addTexture("../objects/bricks_specular.png", "specular");

    // Define objects
    std::vector<Object> objects;
    Object object;
    object.name = "teapot";
    objects.push_back(object);

    object.name = "floor";
    object.position = glm::vec3(0.0f, -0.85f, 0.0f);
    objects.push_back(object);

    object.name = "wall";
    object.scale = glm::vec3(5.0f, 1.0f, 5.0f);
    object.rotation = glm::vec3(1.0f, 0.0f, 0.0f);//define the rotation to be aroundhte x-axis
    object.angle = glm::radians(90.0f);//rotate 90 degrees in radians
    object.position = glm::vec3(0.0f, 4.0f, -2.0f);
    objects.push_back(object);

    // Specify world space object positions
    glm::vec3 positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -10.0f),
        glm::vec3(-3.0f, -2.0f, -4.0f),
        glm::vec3(-4.0f, -2.0f, -8.0f),
        glm::vec3( 2.0f, -1.0f, -4.0f),
        glm::vec3(-4.0f,  3.0f, -10.0f),
        glm::vec3( 0.0f, -2.0f, -8.0f),
        glm::vec3( 4.0f,  2.0f, -6.0f),
        glm::vec3( 3.0f,  0.0f, -1.0f),
        glm::vec3(-1.0f,  1.0f, -2.0f)
    };
    
    // Define light colours
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    
    // Specify light sources
    Light lightSources;
    lightSources.addPointLight(glm::vec3(2.0f, 2.0f, 2.0f), white);
    lightSources.addSpotLight(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), white, cos(glm::radians(45.0f)));
    lightSources.addDirLight(glm::vec3(1.0f, 0.0f, 0.0f), yellow);
    
    // Compile light shader program
    lightSources.lightShaderID = LoadShaders("lightVertexShader.vert", "lightFragmentShader.frag");
    
    do {
        // Update timers
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Background colour
        glm::vec3 bgColour = glm::vec3(0.0f, 0.0f, 0.0f);
        glClearColor(bgColour[0], bgColour[1], bgColour[2], 1.0f);
        
        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get the view and projection matrices from the camera library
        camera.calculateMatrices(window, deltaTime);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        // Activate shader
        glUseProgram(shaderID);
        
        // Send camera position, view and projection matrices to the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Send light source properties to the shader
        lightSources.toShader(shaderID);
    
        // Loop through objects
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            // Calculate model matrix
            glm::mat4 translate = glm::translate(glm::mat4(1.0f), objects[i].position);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), objects[i].scale);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), objects[i].angle, objects[i].rotation);
            glm::mat4 model = translate * rotate * scale;

            // Send the model matrix to the shader
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &model[0][0]);

            // Send material properties to the shader
            glUniform1f(glGetUniformLocation(shaderID, "ka"), objects[i].ka);
            glUniform1f(glGetUniformLocation(shaderID, "kd"), objects[i].kd);
            glUniform1f(glGetUniformLocation(shaderID, "ks"), objects[i].ks);
            glUniform1f(glGetUniformLocation(shaderID, "Ns"), objects[i].Ns);

            // Draw the model
            if (objects[i].name == "teapot")
                teapot.draw(shaderID);
            if (objects[i].name == "floor")
                floor.draw(shaderID);
            if (objects[i].name == "wall")
                wall.draw(shaderID);
        }
        
        // Draw the light sources
        lightSources.draw(view, projection, lightModel);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
