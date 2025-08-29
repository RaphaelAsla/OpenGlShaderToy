#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include "compute_shader.hpp"

namespace fs = std::filesystem;


inline ComputeShader CreateComputeShaderFolder(const std::string& project_name) {
    std::string compute_shader = "../shaders/" + project_name + "/" + project_name + ".comp.glsl";

    if (!fs::exists("../shaders/" + project_name)) {
        fs::create_directory("../shaders/" + project_name);
        std::cout << "Created folder: " << project_name << "\n";
    } else {
        std::cout << "Folder already exists: " << project_name << "\n";
        std::cout << "Continuing with: " << project_name << "\n";
        return ComputeShader{compute_shader};
    }

    std::ofstream compute_shader_file{compute_shader};
    if (compute_shader_file.is_open()) {
        compute_shader_file << "#version 430 core\n\n";
        compute_shader_file << "layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;\n";
        compute_shader_file << "layout(rgba32f, binding = 0) uniform image2D imgOutput;\n\n";
        compute_shader_file << "uniform vec2 resolution;\n";
        compute_shader_file << "uniform vec2 mouse_pos;\n";
        compute_shader_file << "uniform float time;\n\n";
        compute_shader_file << "void main() {\n";
        compute_shader_file << "    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);\n";
        compute_shader_file << "    if (texelCoord.x >= int(resolution.x) || "
                               "texelCoord.y >= int(resolution.y)) return;\n\n";
        compute_shader_file << "    vec2 uv = vec2(texelCoord) / resolution * 2.0 - 1.0;\n";
        compute_shader_file << "    uv.x *= resolution.x / resolution.y;\n\n";
        compute_shader_file << "    vec4 color = vec4(uv.xy, 0.0, 1.0);\n";
        compute_shader_file << "    imageStore(imgOutput, texelCoord, color);\n";
        compute_shader_file << "}\n";
        std::cout << "Created and wrote to file: " << compute_shader << "\n";
    } else {
        std::cout << "Failed to open compute shader file for writing: " << compute_shader << "\n";
    }

    std::string images_folder = "../shaders/" + project_name + "/images";
    if (!fs::exists(images_folder)) {
        fs::create_directory(images_folder);
        std::cout << "Created folder: " << images_folder << "\n";
    } else {
        std::cout << "Images folder already exists: " << images_folder << "\n";
    }

    return ComputeShader{compute_shader};
}
