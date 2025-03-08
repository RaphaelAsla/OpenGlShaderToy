#pragma once

#include <glad/gl.h>

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>

class Shader {
  public:
    unsigned int                    ID;
    std::string                     vertex_path;
    std::string                     fragment_path;
    std::filesystem::file_time_type vertex_last_write_time;
    std::filesystem::file_time_type fragment_last_write_time;

    Shader() = default;

    Shader(const std::string& vertex_path, const std::string& fragment_path) : vertex_path(vertex_path), fragment_path(fragment_path) {
        LoadShader();
        TrackFileModification();
    }

    void Reload() {
        if (ShaderFilesModified()) {
            std::cout << "Reloading shaders...\n";
            LoadShader();
            TrackFileModification();
        }
    }

    void Use() const {
        glUseProgram(ID);
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

    void SetVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

  private:
    void LoadShader() {
        std::string vertexCode, fragmentCode;
        if (!ReadShaderFile(vertex_path, vertexCode) || !ReadShaderFile(fragment_path, fragmentCode)) {
            std::cout << "Failed to read shader files\n";
            return;
        }

        unsigned int new_program = glCreateProgram();
        unsigned int vertex      = CompileShader(GL_VERTEX_SHADER, vertexCode);
        unsigned int fragment    = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

        if (!vertex || !fragment) {
            std::cout << "Shader compilation failed!" << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return;
        }

        glAttachShader(new_program, vertex);
        glAttachShader(new_program, fragment);
        glLinkProgram(new_program);

        if (!CheckCompileErrors(new_program, "PROGRAM")) {
            std::cout << "Shader linking failed" << std::endl;
            glDeleteProgram(new_program);
            return;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        if (ID) {
            glDeleteProgram(ID);
        }

        ID = new_program;
        std::cout << "Shader reloaded successfully!\n";
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

        if (!CheckCompileErrors(shader, (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT"))) {
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    bool ShaderFilesModified() {
        return (std::filesystem::last_write_time(vertex_path) != vertex_last_write_time) || (std::filesystem::last_write_time(fragment_path) != fragment_last_write_time);
    }

    void TrackFileModification() {
        vertex_last_write_time   = std::filesystem::last_write_time(vertex_path);
        fragment_last_write_time = std::filesystem::last_write_time(fragment_path);
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
