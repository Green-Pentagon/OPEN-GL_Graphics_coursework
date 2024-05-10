#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

Camera::Camera(const glm::vec3 position)
{
    this->position = position;
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::getViewMatrix()
{
    return view;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projection;
}

void Camera::calculateMatrices(GLFWwindow *window, const float deltaTime)
{
    // Keyboard inputs (movement)
    if (glfwGetKey(window, GLFW_KEY_W))
        position += front * deltaTime * speed;
    
    if (glfwGetKey(window, GLFW_KEY_S))
        position -= front * deltaTime * speed;
    
    if (glfwGetKey(window, GLFW_KEY_A))
        position -= right * deltaTime * speed;
    
    if (glfwGetKey(window, GLFW_KEY_D))
        position += right * deltaTime * speed;
    
    //Roll
    if (glfwGetKey(window, GLFW_KEY_Q))
        roll -= 0.5f * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_E))
        roll += 0.5f * deltaTime * speed;

    // Get mouse cursor position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // Update yaw and pitch angles
    yaw += mouseSpeed * deltaTime * float(xPos - 1024/2);
    pitch += mouseSpeed * deltaTime * float(yPos - 768/2);

    //// Update camera vectors
    //front = glm::normalize(glm::vec3(sin(yaw) * cos(pitch), -sin(pitch), -cos(yaw) * cos(pitch)));
    //right = glm::normalize(glm::cross(front, worldUp));
    //up = glm::normalize(glm::cross(right, front));
    //target = position + front;
    //
    //// Calculate view matrix
    //view = glm::lookAt(position, target, up);

    // Calculate direction quaternion
    direction.eulerToQuat(pitch, yaw, roll);

    // Calculate view matrix
    view = direction.quatToMat() * Maths::translate(glm::mat4(1.0f), -position);
    
    // Update camera vectors
    right.x = view[0][0], right.y = view[1][0], right.z = view[2][0];
    up.x = view[0][1], up.y = view[1][1], up.z = view[2][1];
    front.x = -view[0][2], front.y = -view[1][2], front.z = -view[2][2];

    // Calculate projection matrix
    projection = MyLib::Perspective(fov, aspect, near, far);
}

void Camera::thirdPersonCamera(GLFWwindow* window, const float deltaTime)
{
    // Keyboard inputs (movement)
    if (glfwGetKey(window, GLFW_KEY_W))
        position += front * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_S))
        position -= front * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_A))
        charYaw -= 0.5f * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_D))
        charYaw += 0.5f * deltaTime * speed;

    //Roll
    if (glfwGetKey(window, GLFW_KEY_Q))
        roll -= 0.5f * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_E))
        roll += 0.5f * deltaTime * speed;

    // Get mouse cursor position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Update yaw and pitch angles
    yaw += mouseSpeed * deltaTime * float(xPos - 1024 / 2);
    pitch += mouseSpeed * deltaTime * float(yPos - 768 / 2);

    // Calculate direction quaternion
    Quaternion newDirection;
    newDirection.eulerToQuat(pitch, yaw, roll);
    direction = Maths::slerp(direction, newDirection, deltaTime);

    // Calculate view matrix
    view = Maths::translate(glm::mat4(1.0f), -offset) * direction.quatToMat() * Maths::translate(glm::mat4(1.0f), -position);

    // Update character movement vectors
    charDirection.eulerToQuat(pitch, charYaw, roll);
    glm::mat4 charMat = charDirection.quatToMat();
    right.x = charMat[0][0], right.y = charMat[1][0], right.z = charMat[2][0];
    up.x = charMat[0][1], up.y = charMat[1][1], up.z = charMat[2][1];
    front.x = -charMat[0][2], front.y = -charMat[1][2], front.z = -charMat[2][2];

    // Calculate projection matrix
    projection = MyLib::Perspective(fov, aspect, near, far);
}