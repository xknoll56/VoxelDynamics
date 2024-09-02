#pragma once
#include <fstream>
#include <sstream>
#include <map>

struct Shader
{

	GLuint programId;

	void LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
			getchar();
			return;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;


		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}



		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}



		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}


		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		programId = ProgramID;
	}

	void use()
	{
		glUseProgram(programId);
	}

	~Shader()
	{
		glDeleteProgram(programId);
	}

	std::map<std::string, GLuint> uniformVector3s;
	std::map<std::string, GLuint> uniformMatrix4s;
	std::map<std::string, GLuint> uniformVector2s;
	std::map<std::string, GLuint> uniformInts;
	std::map<std::string, GLuint> uniformFloats;

	void insertUniformVector3(std::string name)
	{
		GLuint uniformLoc = glGetUniformLocation(programId, name.c_str());
		uniformVector3s[name] = uniformLoc;
	}

	void insertUniformVector2(std::string name)
	{
		GLuint uniformLoc = glGetUniformLocation(programId, name.c_str());
		uniformVector2s[name] = uniformLoc;
	}

	void insertUniformInt(std::string name)
	{
		GLuint uniformLoc = glGetUniformLocation(programId, name.c_str());
		uniformInts[name] = uniformLoc;
	}

	void insertUniformFloat(std::string name)
	{
		GLuint uniformLoc = glGetUniformLocation(programId, name.c_str());
		uniformFloats[name] = uniformLoc;
	}

	void insertUniformMatrix4(std::string name)
	{
		GLuint uniformLoc = glGetUniformLocation(programId, name.c_str());
		uniformMatrix4s[name] = uniformLoc;
	}

	void setUniformVector3(std::string name, VDVector3 value)
	{
		glUniform3f(uniformVector3s[name], value.x, value.y, value.z);
	}

	void setUniformVector2(std::string name, VDVector2 value)
	{
		glUniform2f(uniformVector2s[name], value.x, value.y);
	}

	void setUniformMatrix4(std::string name, VDMatrix value)
	{
		glUniformMatrix4fv(uniformMatrix4s[name], 1, GL_FALSE, &value.m[0][0]);
	}

	void setUniformInt(std::string name, int value)
	{
		glUniform1i(uniformInts[name], value);
	}

	void setUniformFloat(std::string name, float value)
	{
		glUniform1f(uniformInts[name], value);
	}
};


