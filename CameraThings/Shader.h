#pragma once
#include <glad/glad.h>

class Shader
{
private:
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

public:
    void CreateVertexShader(const char* vertexShaderSource);
    void CreateFragmentShader(const char* fragmentShaderSource);
    void LinkProgram();
    unsigned int GetProgram();
};