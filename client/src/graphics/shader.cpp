#include "shader.h"

Shader::Shader(std::string vertS, std::string fragS)
{
	program = glCreateProgram();
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

    vertS = readFile(vertS.c_str());
    fragS = readFile(fragS.c_str());
	const char *vertSource = vertS.c_str();
	const char *fragSource = fragS.c_str();
    
	glShaderSource(vertex, 1, &vertSource, NULL);
	glCompileShader(vertex);
	if (CheckShaderError(vertex, GL_COMPILE_STATUS, false, "Vertex Shader Compile Failed: ")) {
		glDeleteShader(vertex);
	}

	glShaderSource(fragment, 1, &fragSource, NULL);
	glCompileShader(fragment);
	if (CheckShaderError(fragment, GL_COMPILE_STATUS, false, "Fragment Shader Compile Failed: ")) {
		glDeleteShader(fragment);
	}

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Linking Failed: ");
	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Validating Failed: ");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

std::string Shader::readFile(const char* filepath)
{
    using namespace std::string_literals;

    FILE *file = fopen(filepath, "r+b");
    if (file == NULL) {
        Log("nie moge wczytac pliku "s + filepath);
        return "bebebe";
    }
    
    fseek(file, 0, SEEK_END);
    unsigned long length = ftell(file);
    char *data = new char[length + 1];
    memset(data, 0, length + 1);
    fseek(file, 0, SEEK_SET);
    fread(data, 1, length, file);
    fclose(file);

    std::string result(data);
    delete[] data;
    return result;
}

bool Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string &errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);
	if (success == GL_FALSE) {
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		Log(errorMessage + error);
		return true;
	}
	return false;
}
