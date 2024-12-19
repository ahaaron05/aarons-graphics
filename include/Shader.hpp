#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	// generates shaders & program on demand
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// allow for file exceptions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open the files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			// read into stream
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::" <<e.what() << std::endl;
		}
		// convert cpp str into c_str
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// Vertex shader
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "SHADER");
		
		// Fragment shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "SHADER");

		// Shader program
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void use()
	{
		glUseProgram(ID);
	}

	// uniform functionality
	void setBool(const std::string& name, bool value) const // const means we don't modify the actual member variables
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec3(const std::string& name, glm::vec3 value) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
	}
	void setVec4(const std::string& name, glm::vec4 value) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
	}
	void setMat3(const std::string& name, glm::mat3 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void setMat4(const std::string& name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

private:
	void checkCompileErrors(GLuint shader, std::string type)
	{
		int success;
		char infoLog[512];

		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILURE\n" << infoLog << std::endl;
			}
			else
				return;
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, sizeof(infoLog), NULL, infoLog);
				std::cout << "ERROR:SHADER_PROGRAM::LINKING_FAILURE\n" << infoLog << std::endl;
			}
			else
				return;
		}
	}
};