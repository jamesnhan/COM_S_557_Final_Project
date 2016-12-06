#include "Shader.h"

using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::stringstream;

/// <summary> Load, compile, and link vertex and fragment shaders </summary>
/// <param name="VertexPath"> Vertex shader file path </param>
/// <param name="FragmentPath"> Vertex shader file path </param>
Shader::Shader(string VertexPath, string FragmentPath)
{
	// The shader code
	string vertexCode;
	string fragmentCode;

	// Input file streams to read the files
	ifstream vertexFile;
	ifstream fragmentFile;

	// Set the exceptions for our input streams
	vertexFile.exceptions(ifstream::failbit | ifstream::badbit);
	fragmentFile.exceptions(ifstream::failbit | ifstream::badbit);

	// Try to read the files in
	try
	{
		// Open the files for reading
		vertexFile.open(VertexPath);
		fragmentFile.open(FragmentPath);

		// Use stringstream for easy conversion and reading
		stringstream vertexStream;
		stringstream fragmentStream;

		// Read into the stringstream
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		// Close the files
		vertexFile.close();
		fragmentFile.close();

		// Get the code as strings
		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (ifstream::failure e)
	{
		cerr << "Error: Unable to load shaders " << VertexPath << ", " << FragmentPath << endl;
	}

	// Get the code in a C-style string
	const GLchar* vertexShaderCode = vertexCode.c_str();
	const GLchar* fragmentShaderCode = fragmentCode.c_str();

	// Shader program ID's
	GLuint vertex;
	GLuint fragment;

	// Error checking variables
	GLint success;
	GLchar infoLog[512];

	// Create the vertex shader and compile it
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
	glCompileShader(vertex);

	// Check success
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		cerr << "Error: Failed to compile vertex shader\n" << infoLog << endl;
	}

	// Create the fragment shader and compile it
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
	glCompileShader(fragment);

	// Check success
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		cerr << "Error: Failed to compile fragment shader\n" << infoLog << endl;
	}
	
	// Create the shader program
	this->Program = glCreateProgram();

	// Attach both shader programs
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);

	// Link our shader program
	glLinkProgram(this->Program);

	// Check success
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
		cerr << "Error: Failed to link shaders\n" << infoLog << endl;
	}

	// Delete shaders after they have been linked
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

/// <summary> Gets the shader's program ID </summary>
/// <returns> The program ID </returns>
GLuint Shader::GetProgram() const
{
	return this->Program;
}

/// <summary> Begin using our shader </summary>
void Shader::Use() const
{
	glUseProgram(this->Program);

	// Setup every light
	glUniform1i(glGetUniformLocation(this->Program, "numDirectionalLights"), 0);
	glUniform1i(glGetUniformLocation(this->Program, "numPointLights"), 0);
	glUniform1i(glGetUniformLocation(this->Program, "numSpotLights"), 0);
	for (auto&& light : this->Lights)
	{
		// Check if light is enabled
		if (!light->Enabled)
		{
			continue;
		}

		int numLightsLoc = glGetUniformLocation(this->Program, ("num" + light->Type + "Lights").c_str());
		int numLights = 0;

		glGetUniformiv(this->Program, numLightsLoc, &numLights);

		std::string uniformName = light->Type + "Lights[" + std::to_string(numLights++) + "].";

		int positionLocation = glGetUniformLocation(this->Program, (uniformName + "Position").c_str());
		int colorLocation = glGetUniformLocation(this->Program, (uniformName + "Color").c_str());
		int ambientLocation = glGetUniformLocation(this->Program, (uniformName + "Ambient").c_str());
		int diffuseLocation = glGetUniformLocation(this->Program, (uniformName + "Diffuse").c_str());
		int specularLocation = glGetUniformLocation(this->Program, (uniformName + "Specular").c_str());

		glUniform4fv(positionLocation, 1, glm::value_ptr(light->Position));
		glUniform3fv(colorLocation, 1, glm::value_ptr(light->Color));
		glUniform1f(ambientLocation, light->AmbientIntensity);
		glUniform1f(diffuseLocation, light->DiffuseIntensity);
		glUniform1f(specularLocation, light->SpecularIntensity);

		if (light->Type == "Directional")
		{
		}
		else if (light->Type == "Point")
		{
			PointLight* pointLight = static_cast<PointLight*>(light);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "Constant").c_str()), pointLight->Constant);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "Linear").c_str()), pointLight->Linear);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "Quadratic").c_str()), pointLight->Quadratic);
		}
		else if (light->Type == "Spot")
		{
			SpotLight* spotLight = static_cast<SpotLight*>(light);
			glUniform3fv(glGetUniformLocation(this->Program, (uniformName + "Direction").c_str()), 1, glm::value_ptr(spotLight->Direction));
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "Constant").c_str()), spotLight->Constant);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "Linear").c_str()), spotLight->Linear);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "Quadratic").c_str()), spotLight->Quadratic);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "CutOff").c_str()), spotLight->CutOff);
			glUniform1f(glGetUniformLocation(this->Program, (uniformName + "OuterCutOff").c_str()), spotLight->OuterCutOff);
		}

		glUniform1i(numLightsLoc, numLights);
	}
}

void Shader::AddLight(Light* Light)
{
	if (this->Lights.size() < MAX_LIGHTS)
	{
		this->Lights.push_back(Light);
	}
}

void Shader::RemoveLight(Light* Light)
{
	auto iter = this->Lights.begin();
	while (iter != this->Lights.end())
	{
		if (*iter == Light)
		{
			this->Lights.erase(iter);
			break;
		}
		++iter;
	}
}

void Shader::ClearLights()
{
	this->Lights.clear();
}