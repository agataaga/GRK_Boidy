#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "boidSetUp.h"
#include "boids.h" 


// Pyramid vertices
GLfloat pyramidVertices[] = {
    // Base (square)
    -0.1f,  0.0f,  0.1f,  1.0f, 0.0f, 0.0f, // Bottom-left
     0.1f,  0.0f,  0.1f,  0.0f, 1.0f, 0.0f, // Bottom-right
     0.1f,  0.0f, -0.1f,  0.0f, 0.0f, 1.0f, // Top-right
    -0.1f,  0.0f, -0.1f,  1.0f, 1.0f, 0.0f, // Top-left

    // Peak (triangle faces)
     0.0f,  0.2f,  0.0f,  1.0f, 1.0f, 1.0f, // Top (shared)
};

GLuint pyramidIndices[] = {
    // Base
    0, 1, 2,
    0, 2, 3,
    // Sides
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

// OpenGL buffers
GLuint pVAO, pVBO, pEBO;

void setupPyramid() {
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1, &pVBO);
    glGenBuffers(1, &pEBO);

    glBindVertexArray(pVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderBoids(std::vector<Boid>& boids, Shader& shaderProgram) {
    for (Boid& boid : boids) {
        glm::mat4 model = glm::mat4(1.0f);

        /*std::cout << "Boid Position: ("
            << boid.position.x << ", "
            << boid.position.y << ", "
            << boid.position.z << ")" << std::endl;*/

            // Apply position and direction
        model = glm::translate(model, boid.position);
        model = glm::rotate(model, glm::radians(boid.angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f)); // Scale for better visibility

        shaderProgram.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(pVAO);
        glDrawElements(GL_TRIANGLES, sizeof(pyramidIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
    }
}