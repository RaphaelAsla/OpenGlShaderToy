#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>

#include "../includes/shader.hpp"
#include "../includes/utils.hpp"
#include "glad/gl.h"

int WIDTH  = 1000;
int HEIGHT = 800;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    std::string shader_name;
    std::cout << "Give an existing or new shader name: ";
    std::getline(std::cin, shader_name);
    if (shader_name.empty()) {
        std::cerr << "Shader name cannot be empty" << std::endl;
        glfwTerminate();
        return -1;
    }

    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGLShaderToy", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    glfwSetWindowSizeCallback(window, []([[maybe_unused]] auto wnd, int w, int h) {
        WIDTH  = w;
        HEIGHT = h;
        glViewport(0, 0, w, h);
    });

    Shader shader = CreateShaderFolder(shader_name);

    float vertices[] = {
        -1.0f, -1.0f,  // Bottom-left
        1.0f,  -1.0f,  // Bottom-right
        -1.0f, 1.0f,   // Top-left
        1.0f,  1.0f    // Top-right
    };

    unsigned int indices[] = {
        0, 1, 2,  // First triangle
        1, 2, 3   // Second triangle
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::dvec2 mouse_pos;

    while (!glfwWindowShouldClose(window)) {
        shader.Reload();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();

        shader.SetVec2("resolution", WIDTH, HEIGHT);
        shader.SetFloat("time", glfwGetTime());
        shader.SetVec2("mouse_pos", mouse_pos);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
