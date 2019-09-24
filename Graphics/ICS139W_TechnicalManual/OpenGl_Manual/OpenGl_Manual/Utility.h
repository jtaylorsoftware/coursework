#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace util
{
    struct Camera
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };
}