#include<iostream>
#include <glad/glad.h>
#include<glfw3.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "Camera.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include "BoidSetUp.h"
#include "Boids.h"
#include "Box.h"

const unsigned int width = 800;
const unsigned int height = 800;

//wyzwanie dla chetnych - przeniesc te dwa gowna do box.cpp i box.h tak zeby dzialalo 
//bo ja juz sie poddaje, wiec chwilowo niech zasmiecaja maina

GLfloat boxVertices[] = {
    //     COORDINATES      //    COLORS  //      NORMALS    
   -0.1f, -0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // Front face
     0.1f, -0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   1.0f, -1.0f,  // Back face
     0.1f,  0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   1.0f, -1.0f,  // Back face
     -0.1f,  0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // Front face

     -0.1f, -0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // Top face
     0.1f, -0.1f, -0.1f,   0.8f, 0.52f, 0.25f,  0.0f, -1.0f,  // Bottom face
     0.1f,  0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, -1.0f,  // Bottom face
     -0.1f,  0.1f, -0.1f,   0.8f, 0.52f, 0.25f,   0.0f, 1.0f   // Top face
};

GLuint boxIndices[] = {
    // Front face
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    // Back face
    4, 5,
    5, 6,
    6, 7,
    7, 4,

    // Left face
    0, 4,
    1, 5,
    2, 6,
    3, 7,

    // Right face
    0, 5,
    1, 4,
    2, 7,
    3, 6,

    // Top face
    3, 2,
    7, 6,

    // Bottom face
    0, 1,
    4, 5
};


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "mywindow", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);

    // shader for our basic wire cube
    Shader shaderProgram("cube.vert", "cube.frag");
    VAO boxVAO;
    boxVAO.Bind();
    VBO boxVBO(boxVertices, sizeof(boxVertices));
    EBO boxEBO(boxIndices, sizeof(boxIndices));
    // position
    boxVAO.LinkAttrib(boxVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    // color
    boxVAO.LinkAttrib(boxVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //normals - for light
    boxVAO.LinkAttrib(boxVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    boxVAO.Unbind();
    boxVBO.Unbind();
    boxEBO.Unbind();
    shaderProgram.Activate();

    Shader fishShader("fish.vert", "fish.frag");
    fishShader.Activate();

    //set up a pyramid shaped vao, so it represents our boids
    setupPyramid();
    
    // matrixes for the camera
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -30.0f)); // sets how far away we are from the cube
    glEnable(GL_DEPTH_TEST);
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!
    std::vector<Boid> boids; 
    setUpBoids(boids, 5, 80); // set up num of boid groups you want here : boids, num of groups, num of boids in each group

    while (!glfwWindowShouldClose(window)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        processInput(window);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //allows for camera movement
        processInput(window);
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(fishShader.ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        shaderProgram.Activate();
        fishShader.Activate();
        
        camera.Matrix(shaderProgram, "camMatrix");
        camera.Matrix(fishShader, "camMatrix");

        GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        modelLoc = glGetUniformLocation(fishShader.ID, "model");

        GLuint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        viewLoc = glGetUniformLocation(fishShader.ID, "view");

        glm::mat4 identityMatrix = glm::mat4(1.0f);
        model = glm::scale(identityMatrix, glm::vec3(100.0f, 100.0f, 100.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        boxVAO.Bind();
        //draws wire cube for the boids to fly in
        glUniform3fv(glGetUniformLocation(shaderProgram.ID, "color"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawElements(GL_LINES, sizeof(boxIndices)/ sizeof(int), GL_UNSIGNED_INT, 0);
        

        // boid rendering and updating
        for (auto& boid : boids) {
            boid.update(boids); 
        }
        renderBoids(boids, fishShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
