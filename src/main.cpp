#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>

#include "../includes/compute_shader.hpp"
#include "../includes/utils.hpp"
#include "glad/gl.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int WIDTH  = 1000;
int HEIGHT = 800;

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <shader_name>" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::string shader_name = argv[1];

    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);

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
    glfwSwapInterval(0);

    glfwSetWindowSizeCallback(window, [](GLFWwindow* wnd, int w, int h) {
        WIDTH  = w;
        HEIGHT = h;
        glViewport(0, 0, w, h);

        GLuint* output_texture_ptr = static_cast<GLuint*>(glfwGetWindowUserPointer(wnd));
        if (output_texture_ptr) {
            glBindTexture(GL_TEXTURE_2D, *output_texture_ptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
            glBindImageTexture(0, *output_texture_ptr, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        }
    });

    ComputeShader compute_shader = CreateComputeShaderFolder(shader_name);

    GLuint output_texture;
    glGenTextures(1, &output_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, output_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, output_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    float quad_vertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    unsigned int quad_indices[] = {0, 1, 2, 1, 2, 3};

    unsigned int quad_VAO, quad_VBO, quad_EBO;
    glGenVertexArrays(1, &quad_VAO);
    glGenBuffers(1, &quad_VBO);
    glGenBuffers(1, &quad_EBO);

    glBindVertexArray(quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const char* display_vert_source = R"(
#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
})";

    const char* display_frag_source = R"(
#version 430 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D screenTexture;
void main() {
    FragColor = texture(screenTexture, TexCoord);
})";

    unsigned int display_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(display_vertex_shader, 1, &display_vert_source, NULL);
    glCompileShader(display_vertex_shader);

    unsigned int display_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(display_fragment_shader, 1, &display_frag_source, NULL);
    glCompileShader(display_fragment_shader);

    unsigned int display_program = glCreateProgram();
    glAttachShader(display_program, display_vertex_shader);
    glAttachShader(display_program, display_fragment_shader);
    glLinkProgram(display_program);

    glDeleteShader(display_vertex_shader);
    glDeleteShader(display_fragment_shader);

    std::cout << "Setting up ImGui context...\n";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    std::cout << "ImGui setup complete!\n";

    std::cout << "Creating default white texture...\n";
    GLuint        texture;
    unsigned char white_data[4] = {255, 255, 255, 255};

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    std::cout << "Texture setup complete!\n";

    glm::dvec2   mouse_pos;
    static float time_scale    = 1.0f;
    static bool  vsync_enabled = false;

    glfwSetWindowUserPointer(window, &output_texture);

    std::cout << "Entering main render loop...\n";

    while (!glfwWindowShouldClose(window)) {
        compute_shader.Reload();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);

        compute_shader.Use();
        compute_shader.SetVec2("resolution", WIDTH, HEIGHT);
        compute_shader.SetFloat("time", glfwGetTime() * time_scale);
        compute_shader.SetVec2("mouse_pos", mouse_pos);

        compute_shader.Dispatch((WIDTH + 7) / 8, (HEIGHT + 7) / 8, 1);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(display_program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, output_texture);
        glUniform1i(glGetUniformLocation(display_program, "screenTexture"), 0);

        glBindVertexArray(quad_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        {
            ImGui::Begin("Shader Controls");

            ImGui::Text("Shader: %s", shader_name.c_str());
            float framerate = ImGui::GetIO().Framerate;
            ImGui::Text("FPS: %.1f (%.3f ms/frame)", framerate, 1000.0f / framerate);

            if (ImGui::Checkbox("Enable VSync", &vsync_enabled)) {
                glfwSwapInterval(vsync_enabled ? 1 : 0);
            }

            ImGui::SliderFloat("Time Scale", &time_scale, 0.0f, 5.0f);

            ImGui::Text("Resolution: %dx%d", WIDTH, HEIGHT);
            ImGui::Text("Mouse: (%.1f, %.1f)", (float)mouse_pos.x, (float)mouse_pos.y);

            if (ImGui::Button("Reload Shader")) {
                compute_shader.Reload();
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &quad_VAO);
    glDeleteBuffers(1, &quad_VBO);
    glDeleteBuffers(1, &quad_EBO);
    glDeleteTextures(1, &output_texture);
    glDeleteTextures(1, &texture);
    glDeleteProgram(display_program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
