#pragma once

#include <glm/glm.hpp>

struct Quaternion
{
    float w, x, y, z;

    // Constructors
    Quaternion();
    Quaternion(const float w, const float x, const float y, const float z);
    glm::mat4 quatToMat();
    void eulerToQuat(const float pitch, const float yaw, const float roll);
};

class Maths
{
public:
    static float radians(const float);
    static float norm(const glm::vec3);
    static glm::vec3 normalize(const glm::vec3);
    static glm::mat4 translate(const glm::mat4, const glm::vec3);
    static glm::mat4 scale(const glm::mat4, const glm::vec3);
    static glm::mat4 rotate(const glm::mat4 mat, const float angle, const glm::vec3 vector);
    static Quaternion slerp(Quaternion q1, Quaternion q2, const float);
};
