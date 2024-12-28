#pragma once

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void generate_sierpinksi(std::vector<glm::mat4> transformations, glm::mat4 current_transformation, int degree)
{
    if(degree == 0)
    {
        transformations.push_back(current_transformation);
        return;
    }

    glm::mat4 scale = glm::scale(current_transformation, glm::vec3(0.5f));
    generate_sierpinksi(transformations, glm::translate(scale, glm::vec3(-1, -1, 0)), degree--);
    generate_sierpinksi(transformations, glm::translate(scale, glm::vec3(1, -1, 0)), degree--);
    generate_sierpinksi(transformations, glm::translate(scale, glm::vec3(0, 1, 0)), degree--);
}

void setup_sierpinski(std::vector<glm::mat4> transformations, int degree)
{
    transformations.clear(); // Clear previous
    generate_sierpinksi(transformations, glm::mat4(1), degree);
}