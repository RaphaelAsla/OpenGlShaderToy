#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>

#include "../includes/shader.hpp"
#include "glad/gl.h"

int WIDTH  = 1000;
int HEIGHT = 800;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Toy", nullptr, nullptr);

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

    Shader shader("../shaders/vert.glsl", "../shaders/frag.glsl");

    /* I haven't got this working yet */
    glfwSetWindowSizeCallback(window, []([[maybe_unused]] auto wnd, int w, int h) {
        WIDTH  = w;
        HEIGHT = h;
        glViewport(0, 0, w, h);
    });

    float vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float      time = 0.0f;
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

        time += 0.016f;
        shader.SetFloat("time", time);
        shader.SetVec2("mouse_pos", mouse_pos.x, mouse_pos.y);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
