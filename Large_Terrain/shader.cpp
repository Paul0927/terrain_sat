#include "shader.h"

Shader::Shader() {
	Program = 0;
	handles.clear();
}

void Shader::attach(int type, const GLchar* filename) {
	string tcode;
	try {
		ifstream shaderFile;
		shaderFile.open(filename);
		stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		tcode = shaderStream.str();
		//cout << tcode << endl;
	}
	catch (ifstream::failure e) {
		cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		exit(1);
	}

	const GLchar* code = tcode.c_str();
	GLint handle;
	GLint success;
	GLchar infoLog[512];
	handle = glCreateShader(type);
	glShaderSource(handle, 1, &code, nullptr);
	glCompileShader(handle);

	//Compile info
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(handle, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
		exit(1);
	}
	handles.push_back(handle);
}

void Shader::link() {
	Program = glCreateProgram();
	for (vector<GLint>::size_type i = 0; i < handles.size(); i++) {
		glAttachShader(Program, handles[i]);
	}
	glLinkProgram(this->Program);
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		exit(1);
	}
	for (auto i : handles)
		glDeleteShader(i);
}

void Shader::begin() {
	glUseProgram(this->Program);
}

void Shader::end() {
	glUseProgram(0);
}