#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Shader {
	public:
		unsigned int ID;

		Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
			/* Retrieving the shader source code from the files */
			std::string vertexCode = readShaderCodeFile(vertexPath);
			std::string fragmentCode = readShaderCodeFile(fragmentPath);
			const char *vShaderCode = vertexCode.c_str();
			const char *fShaderCode = fragmentCode.c_str();

			/* Compile shaders */
			unsigned int vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
			unsigned int fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);

			/* Create program */
			createProgram(vertex, fragment);	
		}
		
		void use() { glUseProgram(this->ID); }

		void setBool(const std::string &name, bool value) const {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
		}
		void setInt(const std::string &name, int value) const {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}
		void setFloat(const std::string &name, float value) const {
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
	
	private:
		std::string readShaderCodeFile(const GLchar *vertexPath) {
			std::string code;
			std::ifstream file;
			file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				file.open(vertexPath);
				/* Read file buffers into streams */
				std::stringstream stream;
				stream << file.rdbuf();

				file.close();
				
				/* Convert the stream to string */
				code = stream.str();
			} catch(std::ifstream::failure *e) {
				std::printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::%s\n", vertexPath);
			}

			return code;
		}

		unsigned int compileShader(const char *shader, unsigned int type) {
			unsigned int compShader;

			/* Create the object */
			compShader = glCreateShader(type);
			/* Attach the source code to the shader object */
			glShaderSource(compShader, 1, &shader, NULL);
			/* Compile the shader */
			glCompileShader(compShader);
			shaderCompilationErrorCheck(compShader);
			return compShader;
		}

		void shaderCompilationErrorCheck(unsigned int shader) {
			int success;
			char infoLog[512];

			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::printf("ERROR::SHADER::%d::COMPILATION_FAILED\n", shader);
				std::printf(infoLog);
			}
		}

		void createProgram(unsigned int vertex, unsigned int fragment) {
			this->ID = glCreateProgram();
			glAttachShader(this->ID, vertex);
			glAttachShader(this->ID, fragment);
			glLinkProgram(this->ID);

			programCreationErrorCheck(this->ID);

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		void programCreationErrorCheck(unsigned int program) {
			int success;
			char infoLog[512];
			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if(!success) {
				glGetProgramInfoLog(program, 512, NULL, infoLog);
				std::printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
			}
		}
};
#endif