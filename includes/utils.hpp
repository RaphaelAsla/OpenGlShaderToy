#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include "shader.hpp"

namespace fs = std::filesystem;

inline Shader CreateShaderFolder(const std::string& project_name) {
    std::string vert_shader = "../shaders/" + project_name + "/" + project_name + ".vert.glsl";
    std::string frag_shader = "../shaders/" + project_name + "/" + project_name + ".frag.glsl";

    if (!fs::exists("../shaders/" + project_name)) {
        fs::create_directory("../shaders/" + project_name);
        std::cout << "Created folder: " << project_name << "\n";
    } else {
        std::cout << "Folder already exists: " << project_name << "\n";
        std::cout << "Continuing with: " << project_name << "\n";
        return Shader{vert_shader, frag_shader};
    }

    std::ofstream vertex_shader_files{vert_shader};
    std::ofstream fragment_shader_files{frag_shader};

    std::ofstream vertex_shader_file{vert_shader};
    if (vertex_shader_file.is_open()) {
        vertex_shader_file << "#version 430 core\n\n";
        vertex_shader_file << "layout(location = 0) in vec2 aPos;\n\n";
        vertex_shader_file << "void main() {\n";
        vertex_shader_file << "    gl_Position = vec4(aPos, 0.0, 1.0);\n";
        vertex_shader_file << "}\n";
        std::cout << "Created and wrote to file: " << vert_shader << "\n";
    } else {
        std::cout << "Failed to open vertex shader file for writing: " << vert_shader << "\n";
    }

    std::ofstream fragment_shader_file{frag_shader};
    if (fragment_shader_file.is_open()) {
        fragment_shader_file << "#version 430 core\n\n";
        fragment_shader_file << "out vec4 fragColor;\n";
        fragment_shader_file << "uniform vec2 resolution;\n";
        fragment_shader_file << "uniform vec2 mouse_pos;\n";
        fragment_shader_file << "uniform float time;\n\n";
        fragment_shader_file << "void main() {\n";
        fragment_shader_file << "    vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;\n";
        fragment_shader_file << "    uv.x *= resolution.x / resolution.y;\n";
        fragment_shader_file << "    fragColor = vec4(uv.xy, 0.0, 1.0);\n";
        fragment_shader_file << "}\n";
        std::cout << "Created and wrote to file: " << frag_shader << "\n";
    } else {
        std::cout << "Failed to open fragment shader file for writing: " << frag_shader << "\n";
    }

    std::cout << "Created file: " << vert_shader << "\n";
    std::cout << "Created file: " << frag_shader << "\n";

    std::string images_folder = "../shaders/" + project_name + "/images";
    if (!fs::exists(images_folder)) {
        fs::create_directory(images_folder);
        std::cout << "Created folder: " << images_folder << "\n";
    } else {
        std::cout << "Images folder already exists: " << images_folder << "\n";
    }

    return Shader{vert_shader, frag_shader};
}
