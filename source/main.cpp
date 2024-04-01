#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "MyLib.h"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

// Create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

// Timers
float currentTime = 0.0f;
float lastTime = 0.0f;
float deltaTime = 0.0f;

// Define object properties
float ka = 0.2f;    // ambient constant
float kd = 0.7f;    //diffuse constant
float ks = 1.0f;    //specular constant
float Ns = 20.0f;   //specular exponent

// Light structs
struct Light
{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant, linear, quadratic;
};

struct SpotLight
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float cosPhi;
	float linear, constant, quadratic;
};

struct DirLight
{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
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
    GLFWwindow* window = glfwCreateWindow( 1024, 768, "Hello Window", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
	// Background colour
	glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
	glClearColor(black[0], black[1], black[2], 1.0f);
    
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Use back face culling
	glEnable(GL_CULL_FACE);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compile shader programs
	GLuint shaderID = LoadShaders("vertexShader.vert", "multipleLightsFragmentShader.frag");
	GLuint lightShaderID = LoadShaders("lightVertexShader.vert", "lightfragmentShader.frag");

	// Load models
	Model teapot("../objects/teapot.obj");
	Model sphere("../objects/sphere.obj");
	Model mouse("../objects/mouse.obj");

	// Add texture to teapot object
	teapot.addTexture("../objects/blue.bmp", "diffuse");
	mouse.addTexture("../objects/crate.bmp", "diffuse");


	// Define light colours
	glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);

	// Create vector of Light structs
	std::vector<Light> lights;

	// Add first light source
	Light light;
	light.position = glm::vec3(2.0f, 2.0f, 2.0f);
	light.ambient = ka * white;
	light.diffuse = kd * white;
	light.specular = ks * white;
	light.constant = 1.0f;
	light.linear = 0.1f;
	light.quadratic = 0.02f;
	lights.push_back(light);

	// Add second light source
	light.position = glm::vec3(1.0f, 1.0f, -8.0f);
	light.ambient = ka * white;
	light.diffuse = kd * white;
	light.specular = ks * white;
	light.constant = 1.0f;
	light.linear = 0.1f;
	light.quadratic = 0.02f;
	lights.push_back(light);


	// Create vector of SpotLight structs
	std::vector<SpotLight> spotLights;

	// Add spotlight
	SpotLight spotLight;
	spotLight.position = glm::vec3(0.0f, 3.0f, 0.0f);
	spotLight.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	spotLight.ambient = ka * white;
	spotLight.diffuse = kd * white;
	spotLight.specular = ks * white;
	spotLight.cosPhi = cos(MyLib::Radians(45.0f));
	spotLight.constant = 1.0f;
	spotLight.linear = 0.1f;
	spotLight.quadratic = 0.02f;
	spotLights.push_back(spotLight);


	// Define directional light
	std::vector<DirLight> dirLights;
	glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
	DirLight dirLight;
	dirLight.direction = glm::vec3(1.0f, -1.0f, 0.0f);
	dirLight.ambient = ka * yellow;
	dirLight.diffuse = kd * yellow;
	dirLight.specular = ks * yellow;
	dirLights.push_back(dirLight);


	// Specify world space object positions
	glm::vec3 positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -10.0f),
		glm::vec3(-3.0f, -2.0f, -4.0f),
		glm::vec3(-4.0f, -2.0f, -8.0f),
		glm::vec3(2.0f, -1.0f, -4.0f),
		glm::vec3(-4.0f,  3.0f, -10.0f),
		glm::vec3(0.0f, -2.0f, -8.0f),
		glm::vec3(4.0f,  2.0f, -6.0f),
		glm::vec3(3.0f,  0.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -2.0f)
	};


    // Render loop
	do {
		// Update timers
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		

		// Clear the window
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get the view and projection matrices from the camera library
		camera.calculateMatrices(window, deltaTime);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		// Activate shader
		glUseProgram(shaderID);

		// Send view and projection matrices to the shaders
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &projection[0][0]);

		// Send material (object) properties to the shader
		glUniform1f(glGetUniformLocation(shaderID, "Ns"), 20.0f);

		//MULTIPLE LIGHT SOURCES
		// Send light source properties to the shader
		glUniform1i(glGetUniformLocation(shaderID, "numLights"), static_cast<unsigned int>(lights.size()));
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			std::string number = std::to_string(i);
			glm::vec3 viewSpaceLightPosition = glm::vec3(view * glm::vec4(lights[i].position, 1.0f));
			glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].ambient").c_str()), 1, &lights[i].ambient[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].diffuse").c_str()), 1, &lights[i].diffuse[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].specular").c_str()), 1, &lights[i].specular[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].position").c_str()), 1, &viewSpaceLightPosition[0]);
			glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].constant").c_str()), lights[i].constant);
			glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].linear").c_str()), lights[i].linear);
			glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].quadratic").c_str()), lights[i].quadratic);
		}

		// Send spotlights to the shader
		glUniform1i(glGetUniformLocation(shaderID, "numSpotLights"), static_cast<unsigned int>(spotLights.size()));
		for (unsigned int i = 0; i < spotLights.size(); i++)
		{
			std::string number = std::to_string(i);
			glm::vec3 viewSpaceSpotLightPosition = glm::vec3(view * glm::vec4(spotLights[i].position, 1.0f));
			glm::vec3 viewSpaceSpotLightDirection = glm::mat3(view) * spotLights[i].direction;
			glUniform3fv(glGetUniformLocation(shaderID, ("spotLights[" + number + "].position").c_str()), 1, &viewSpaceSpotLightPosition[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("spotLights[" + number + "].direction").c_str()), 1, &viewSpaceSpotLightDirection[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("spotLights[" + number + "].ambient").c_str()), 1, &spotLights[i].ambient[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("spotLights[" + number + "].diffuse").c_str()), 1, &spotLights[i].diffuse[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("spotLights[" + number + "].specular").c_str()), 1, &spotLights[i].specular[0]);
			glUniform1f(glGetUniformLocation(shaderID, ("spotLights[" + number + "].cosPhi").c_str()), spotLights[i].cosPhi);
			glUniform1f(glGetUniformLocation(shaderID, ("spotLights[" + number + "].constant").c_str()), spotLights[i].constant);
			glUniform1f(glGetUniformLocation(shaderID, ("spotLights[" + number + "].linear").c_str()), spotLights[i].linear);
			glUniform1f(glGetUniformLocation(shaderID, ("spotLights[" + number + "].quadratic").c_str()), spotLights[i].quadratic);
		}

		// Send directional lights to the shader
		glUniform1i(glGetUniformLocation(shaderID, "numDirLights"), static_cast<unsigned int>(dirLights.size()));
		for (unsigned int i = 0; i < dirLights.size(); i++)
		{
			std::string number = std::to_string(i);
			glm::vec3 viewSpaceDirLightDirection = glm::mat3(view) * dirLights[i].direction;
			glUniform3fv(glGetUniformLocation(shaderID, ("dirLights[" + number + "].direction").c_str()), 1, &viewSpaceDirLightDirection[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("dirLights[" + number + "].ambient").c_str()), 1, &dirLights[i].ambient[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("dirLights[" + number + "].diffuse").c_str()), 1, &dirLights[i].diffuse[0]);
			glUniform3fv(glGetUniformLocation(shaderID, ("dirLights[" + number + "].specular").c_str()), 1, &dirLights[i].specular[0]);
		}

		// Loop through objects
		for (unsigned int i = 0; i < 10; i++)
		{
			// Calculate model matrix
			glm::mat4 model = MyLib::Transform(glm::mat4(1.0f), positions[i], glm::vec3(1.0f), 30.0f * i, glm::vec3(1.0f));

			// Send the model matrix to the shader
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &model[0][0]);

			// Draw the model
			if (i % 2 == 0) {
				teapot.draw(shaderID);
			}
			else
			{
				mouse.draw(shaderID);
			}

		}

		//MULTIPLE LIGHT SOURCES
		// Draw light sources
		glUseProgram(lightShaderID);
		for (unsigned int i = 0; i < lights.size(); i++)
		{
			// Calculate model matrix
			glm::mat4 model = MyLib::Transform(glm::mat4(1.0f), lights[i].position, glm::vec3(1.0f), 0.0f, glm::vec3(0.2f));
			
			// Send model, view, projection matrices and light colour to light shader
			glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniform3fv(glGetUniformLocation(lightShaderID, "lightColour"), 1, &lights[i].specular[0]);

			// Draw light source
			sphere.draw(lightShaderID);
		}

		//Draw Spotlight sources
		for (unsigned int i = 0; i < spotLights.size(); i++)
		{
			// Calculate model matrix
			glm::mat4 model = MyLib::Transform(glm::mat4(1.0f), spotLights[i].position, glm::vec3(1.0f), 0.0f, glm::vec3(0.2f));

			// Send model, view, projection matrices and light colour to light shader
			glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "projection"), 1, GL_FALSE, &projection[0][0]);
			glUniform3fv(glGetUniformLocation(lightShaderID, "lightColour"), 1, &spotLights[i].specular[0]);

			// Draw light source
			sphere.draw(lightShaderID);
		}

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
