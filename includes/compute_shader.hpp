#pragma once

#include <glad/gl.h>

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>

class ComputeShader {
  public:
    unsigned int                    ID;
    std::string                     compute_path;
    std::filesystem::file_time_type compute_last_write_time;

    ComputeShader() = default;

    ComputeShader(const std::string& compute_path) : compute_path(compute_path) {
        LoadShader();
        TrackFileModification();
    }

    void Reload() {
        if (ShaderFileModified()) {
            std::cout << "Reloading compute shader...\n";
            LoadShader();
            TrackFileModification();
        }
    }

    void Use() const {
        glUseProgram(ID);
    }

    void Dispatch(int x, int y, int z = 1) const {
        glDispatchCompute(x, y, z);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void SetBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void SetInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void SetVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

  private:
    void LoadShader() {
        std::string computeCode;
        if (!ReadShaderFile(compute_path, computeCode)) {
            std::cout << "Failed to read compute shader file: " << compute_path << "\n";
            return;
        }

        std::cout << "Read compute shader file successfully, size: " << computeCode.length() << " bytes\n";

        unsigned int new_program = glCreateProgram();
        unsigned int compute     = CompileShader(GL_COMPUTE_SHADER, computeCode);

        if (!compute) {
            std::cout << "Compute shader compilation failed!" << std::endl;
            glDeleteShader(compute);
            return;
        }

        std::cout << "Compute shader compiled successfully\n";

        glAttachShader(new_program, compute);
        glLinkProgram(new_program);

        if (!CheckCompileErrors(new_program, "PROGRAM")) {
            std::cout << "Compute shader linking failed" << std::endl;
            glDeleteProgram(new_program);
            return;
        }

        std::cout << "Compute shader linked successfully\n";

        glDeleteShader(compute);

        if (ID) {
            glDeleteProgram(ID);
        }

        ID = new_program;
        std::cout << "Compute shader reloaded successfully!\n";
    }

    bool ReadShaderFile(const std::string& path, std::string& code) {
        std::ifstream shaderFile(path);
        if (!shaderFile) {
            std::cerr << "Error: Could not open shader file " << path << "\n";
            return false;
        }
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        code = shaderStream.str();
        return true;
    }

    unsigned int CompileShader(GLenum type, const std::string& source) {
        unsigned int shader = glCreateShader(type);
        const char*  src    = source.c_str();
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        if (!CheckCompileErrors(shader, "COMPUTE")) {
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    bool ShaderFileModified() {
        return std::filesystem::last_write_time(compute_path) != compute_last_write_time;
    }

    void TrackFileModification() {
        compute_last_write_time = std::filesystem::last_write_time(compute_path);
    }

    bool CheckCompileErrors(GLuint shader, std::string type) {
        GLint  success;
        GLchar infoLog[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR (" << type << "):\n" << infoLog << "\n";
                return false;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR:\n" << infoLog << "\n";
                return false;
            }
        }

        return true;
    }
};
