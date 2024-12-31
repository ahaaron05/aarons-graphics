#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#ifndef M_PI 	// manually defined pi constant for use in calculations
#define M_PI 3.14159265358979323846
#endif

///////////////////////////
// Sphere: 3D closed surface where every point is same distance from a given point. 
//              x^2 + y^2 + z^2 = r^2
///////////////////////////     TODO


// class Sphere 
// {
// public:

//     Sphere();

//     void initializeData()
//     {
//         // pre-calc sines/cosines for given number of stacks
//         const auto stackAngleStep = -M_PI / float(nStacks);
//         auto currStackAngle = M_PI / 2.0f;
//         std::vector stackSines, stackCosines;
//         for(auto i = 0; i <= nStacks; i++)
//         {

//         }
//     }
// };