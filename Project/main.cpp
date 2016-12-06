/*
 * COM S 557 - Final Project
 * James Nhan (160149990)
 */

// STL
#include <iostream>
#include <string>
#include <random>
#include <ctime>

// GLEW
#include <GL/glew.h>

// GLM
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// GLFW
#include <GLFW/glfw3.h>

// SOIL
#include "SOIL.h"

// Engine Files
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Lights.h"
#include "Plane.h"
#include "CoordinateSystem.h"

using namespace std;

// Handle to the window
GLFWwindow* window;

// Window properties
const char* WINDOW_TITLE = "COM S 557 - James Nhan - Final Project";
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Global camera object
// TODO: Secondary cameras
Camera camera(glm::vec3(0.0f, 0.5f, 0.0f));
bool keys[1024];
GLfloat lastX = 400;
GLfloat lastY = 300;
bool firstMouse = true;
SpotLight mouseFlashLight;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Monster Data
#define MONSTER_IDLE_1 0
#define MONSTER_IDLE_2 1
#define MONSTER_IDLE_3 2

#define MONSTER_RUN_1 3
#define MONSTER_RUN_15 4
#define MONSTER_RUN_2 5
#define MONSTER_RUN_25 6
#define MONSTER_RUN_3 7
#define MONSTER_RUN_35 8
#define MONSTER_RUN_4 9
#define MONSTER_RUN_45 10

#define MONSTER_IDLE_1_TO_IDLE_2 0
#define MONSTER_IDLE_2_TO_IDLE_1 1
#define MONSTER_IDLE_1_TO_IDLE_3 2
#define MONSTER_IDLE_3_TO_IDLE_1 3

#define MONSTER_RUN_1_TO_RUN_15 4
#define MONSTER_RUN_15_TO_RUN_2 5
#define MONSTER_RUN_2_TO_RUN_25 6
#define MONSTER_RUN_25_TO_RUN_3 7
#define MONSTER_RUN_3_TO_RUN_35 8
#define MONSTER_RUN_35_TO_RUN_4 9
#define MONSTER_RUN_4_TO_RUN_45 10
#define MONSTER_RUN_45_TO_RUN_1 11

#define MONSTER_RUN_1_TO_IDLE_1 12
#define MONSTER_RUN_15_TO_IDLE_1 13
#define MONSTER_RUN_2_TO_IDLE_1 14
#define MONSTER_RUN_25_TO_IDLE_1 15
#define MONSTER_RUN_3_TO_IDLE_1 16
#define MONSTER_RUN_35_TO_IDLE_1 17
#define MONSTER_RUN_4_TO_IDLE_1 18
#define MONSTER_RUN_45_TO_IDLE_1 19

#define MONSTER_IDLE_1_TO_RUN_1 20
#define MONSTER_IDLE_2_TO_RUN_1 21
#define MONSTER_IDLE_3_TO_RUN_1 22

int currentAnimation = MONSTER_IDLE_1;
int nextAnimation = MONSTER_IDLE_2;
Model* currentModel;
Model* monsterModels;
int monsterAnimModelIndex[23] = { MONSTER_IDLE_2, MONSTER_IDLE_1, MONSTER_IDLE_3, MONSTER_IDLE_1,
	MONSTER_RUN_15, MONSTER_RUN_2, MONSTER_RUN_25, MONSTER_RUN_3, MONSTER_RUN_35, MONSTER_RUN_4, MONSTER_RUN_45, MONSTER_RUN_1,
MONSTER_RUN_15, MONSTER_RUN_2, MONSTER_RUN_25, MONSTER_RUN_3, MONSTER_RUN_35, MONSTER_RUN_4, MONSTER_RUN_45, MONSTER_RUN_1,
MONSTER_RUN_1, MONSTER_RUN_1, MONSTER_RUN_1 };
GLfloat* vectorDiff[23];
GLfloat* vectorResult;
glm::vec3 monsterPosition;
bool monsterCanSeeYou = false;
float monsterAccel = 0.001f;
float monsterFOV = 37.5f;
Model BrickFloor;


void error_callback(int Error, const char* Description)
{
	cerr << "Error: " << Description << endl;
}

bool Raycast(glm::vec3& ray_start, const glm::vec3&  ray_stop, Model& object, vector<glm::vec3>& intersect_list)
{
    // get the current model matrix.
    glm::mat4 m = object.GetMatrix();
    glm::mat4 mInv = glm::inverse(m);
    
    // get all verticees
    vector<glm::vec3> vertices = object.GetVertices();
    
    
    glm::vec3 s = ray_start;
    glm::vec3 e = ray_stop;

    bool ret = false;
    
    // for all vertices
    vector<glm::vec3>::iterator itr = vertices.begin();
    
    while (itr != vertices.end())
    {
        // get one vertex
        glm::vec3 p0 = (*itr);itr++;
        glm::vec3 p1 = (*itr);itr++;
        glm::vec3 p2 = (*itr);itr++;
        
     
        glm::vec4 v0 = m * glm::vec4(p0.x, p0.y, p0.z, 1.0);
        glm::vec4 v1 = m * glm::vec4(p1.x, p1.y, p1.z, 1.0);
        glm::vec4 v2 = m * glm::vec4(p2.x, p2.y, p2.z, 1.0);
        
        p0.x = v0.x; p0.y = v0.y; p0.z = v0.z;
        p1.x = v1.x; p1.y = v1.y; p1.z = v1.z;
        p2.x = v2.x; p2.y = v2.y; p2.z = v2.z;
        
        
        // prepare the matrix a;
        glm::mat3 A = glm::mat3();
        A[0][0] = s.x - e.x;  A[0][1] = p1.x - p0.x;  A[0][2] = p2.x - p0.x;
        A[1][0] = s.y - e.y;  A[1][1] = p1.y - p0.y;  A[1][2] = p2.y - p0.y;
        A[2][0] = s.z - e.z;  A[2][1] = p1.z - p0.z;  A[2][2] = p2.z - p0.z;
        
        // invert A;
        glm::mat3 invA = glm::inverse(A);
        
        // prepare the vector [t,u,v]
        glm::vec3 tuv;
        tuv.x = s.x - p0.x;
        tuv.y = s.y - p0.y;
        tuv.z = s.z - p0.z;
        
        // multiply
        glm::vec3 result = glm::transpose(invA) * tuv;

        
        float t = result.x;
        float u = result.y;
        float v = result.z;
        
        
        
        // Check if we are still within the triangle.
        if(u>=0.0 && u<= 1.0 && v >=0.0 & v<=1.0)
        {
            float uv = u + v;
            if(uv >= 0.0 && uv <= 1.0  && t >= 0.0 && t <= 1.0)
            {

                // intersetcion
                glm::vec3 x = s + (e - s)*t;
        
                // cout << x.x << " : " << x.y << " : " << x.z << endl;
                //cout << t << " ->  "<< x.x << " : " << x.y << " : " << x.z << endl;
                intersect_list.push_back(x);
                
                ret = true;
            }
        }
		if (itr != vertices.end())
		{
			--itr;
			--itr;
		}
    }
    
    return ret;
}

void DoMovement()
{
	vector<glm::vec3> intersections;
	Raycast(camera.Position, camera.Position - glm::vec3(0.0f, -20.0f, 0.0f), BrickFloor, intersections);
	GLfloat height = 0.5f;
	if (intersections.size() > 0)
	{
		height = intersections[0].y + 0.5f;
	}
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime, height);
	}

	if (keys[GLFW_KEY_S])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime, height);
	}

	if (keys[GLFW_KEY_A])
	{
		camera.ProcessKeyboard(LEFT, deltaTime, height);
	}

	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime, height);
	}
}

void mouse_button_callback(GLFWwindow* Window, int Button, int Action, int Mods)
{
	if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS)
	{
		mouseFlashLight.Enabled = !mouseFlashLight.Enabled;
	}
}

void key_callback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, GL_TRUE);
	}

	if (Action == GLFW_PRESS)
	{
		keys[Key] = true;
	}
	else if (Action == GLFW_RELEASE)
	{
		keys[Key] = false;
	}
}

void mouse_callback(GLFWwindow* Window, double XPos, double YPos)
{
	if (firstMouse)
	{
		lastX = XPos;
		lastY = YPos;
		firstMouse = false;
	}

	GLfloat xoffset = XPos - lastX;
	GLfloat yoffset = lastY - YPos;

	lastX = XPos;
	lastY = YPos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* Window, double XOffset, double YOffset)
{
	camera.ProcessMouseScroll(YOffset);
}

GLuint LoadCubeMap(vector<const GLchar*> faces)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glActiveTexture(texID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	for (GLuint i = 0; i < faces.size(); ++i)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texID;
}

inline void GetRotation(glm::mat4 Matrix, float& Yaw, float& Pitch, float& Roll)
{
	if (Matrix[0][0] == 1.0f || Matrix[0][0] == -1.0f)
	{
		Yaw = atan2f(Matrix[2][0], Matrix[3][2]);
		Pitch = 0.0f;
		Roll = 0.0f;
	}
	else
	{
		Yaw = atan2f(-Matrix[0][2], Matrix[0][0]);
		Pitch = asinf(Matrix[0][1]);
		Roll = atan2f(-Matrix[2][1], Matrix[1][1]);
	}
}

float GetTimeFraction(const float time, const float duration)
{
	float interval = floor(time / duration);
	float currentInterval = time - interval * duration;
	float fraction = currentInterval / duration;

	return fraction;
}

void InitMorphing()
{
	// Idle 1 -> Idle 2
	vectorDiff[MONSTER_IDLE_1_TO_IDLE_2] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_1].Size()));
	// Idle 2 -> Idle 1
	vectorDiff[MONSTER_IDLE_2_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_2].Size()));
	// Idle 1 -> Idle 3
	vectorDiff[MONSTER_IDLE_1_TO_IDLE_3] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_1].Size()));
	// Idle 3 -> Idle 1
	vectorDiff[MONSTER_IDLE_3_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_3].Size()));

	// Run 1 -> Run 15
	vectorDiff[MONSTER_RUN_1_TO_RUN_15] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_1].Size()));
	// Run 15 -> Run 2
	vectorDiff[MONSTER_RUN_15_TO_RUN_2] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_15].Size()));
	// Run 2 -> Run 25
	vectorDiff[MONSTER_RUN_2_TO_RUN_25] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_2].Size()));
	// Run 25 -> Run 3
	vectorDiff[MONSTER_RUN_25_TO_RUN_3] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_25].Size()));
	// Run 3 -> Run 35
	vectorDiff[MONSTER_RUN_3_TO_RUN_35] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_3].Size()));
	// Run 35 -> Run 4
	vectorDiff[MONSTER_RUN_35_TO_RUN_4] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_35].Size()));
	// Run 4 -> Run 45
	vectorDiff[MONSTER_RUN_4_TO_RUN_45] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_4].Size()));
	// Run 45 -> Run 1
	vectorDiff[MONSTER_RUN_45_TO_RUN_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_45].Size()));

	// Run 1 -> Idle 1
	vectorDiff[MONSTER_RUN_1_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_1].Size()));
	// Run 15 -> Idle 1
	vectorDiff[MONSTER_RUN_15_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_15].Size()));
	// Run 2 -> Idle 1
	vectorDiff[MONSTER_RUN_2_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_2].Size()));
	// Run 25 -> Idle 1
	vectorDiff[MONSTER_RUN_25_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_25].Size()));
	// Run 3 -> Idle 1
	vectorDiff[MONSTER_RUN_3_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_3].Size()));
	// Run 35 -> Idle 3
	vectorDiff[MONSTER_RUN_35_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_35].Size()));
	// Run 4 -> Idle 1
	vectorDiff[MONSTER_RUN_4_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_4].Size()));
	// Run 45 -> Idle 4
	vectorDiff[MONSTER_RUN_45_TO_IDLE_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_RUN_45].Size()));

	// Idle 1 -> Run 1
	vectorDiff[MONSTER_IDLE_1_TO_RUN_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_1].Size()));
	// Idle 2 -> Run 1
	vectorDiff[MONSTER_IDLE_2_TO_RUN_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_2].Size()));
	// Idle 3 -> Run 1
	vectorDiff[MONSTER_IDLE_3_TO_RUN_1] = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * monsterModels[MONSTER_IDLE_3].Size()));

	// Idle 1
	int size = monsterModels[MONSTER_IDLE_1].Size();
	vector<glm::vec3> v1 = monsterModels[MONSTER_IDLE_1].GetVertices();
	vector<glm::vec3> v2 = monsterModels[MONSTER_IDLE_2].GetVertices();
	vector<glm::vec3> v3 = monsterModels[MONSTER_RUN_1].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Idle 2
		vectorDiff[MONSTER_IDLE_1_TO_IDLE_2][i * 3 + 0] = v2[i].x - v1[i].x;
		vectorDiff[MONSTER_IDLE_1_TO_IDLE_2][i * 3 + 1] = v2[i].y - v1[i].y;
		vectorDiff[MONSTER_IDLE_1_TO_IDLE_2][i * 3 + 2] = v2[i].z - v1[i].z;

		// To Run 1
		vectorDiff[MONSTER_IDLE_1_TO_RUN_1][i * 3 + 0] = v3[i].x - v1[i].x;
		vectorDiff[MONSTER_IDLE_1_TO_RUN_1][i * 3 + 1] = v3[i].y - v1[i].y;
		vectorDiff[MONSTER_IDLE_1_TO_RUN_1][i * 3 + 2] = v3[i].z - v1[i].z;
	}

	// Idle 2
	size = monsterModels[MONSTER_IDLE_2].Size();
	for (int i = 0; i < size; ++i)
	{
		// To Idle 1
		vectorDiff[MONSTER_IDLE_2_TO_IDLE_1][i * 3 + 0] = v1[i].x - v2[i].x;
		vectorDiff[MONSTER_IDLE_2_TO_IDLE_1][i * 3 + 1] = v1[i].y - v2[i].y;
		vectorDiff[MONSTER_IDLE_2_TO_IDLE_1][i * 3 + 2] = v1[i].z - v2[i].z;

		// To Run 1
		vectorDiff[MONSTER_IDLE_2_TO_RUN_1][i * 3 + 0] = v3[i].x - v2[i].x;
		vectorDiff[MONSTER_IDLE_2_TO_RUN_1][i * 3 + 1] = v3[i].y - v2[i].y;
		vectorDiff[MONSTER_IDLE_2_TO_RUN_1][i * 3 + 2] = v3[i].z - v2[i].z;
	}

	// Idle 3
	size = monsterModels[MONSTER_IDLE_3].Size();
	v1 = monsterModels[MONSTER_IDLE_3].GetVertices();
	v2 = monsterModels[MONSTER_IDLE_1].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Idle 1
		vectorDiff[MONSTER_IDLE_3_TO_IDLE_1][i * 3 + 0] = v2[i].x - v1[i].x;
		vectorDiff[MONSTER_IDLE_3_TO_IDLE_1][i * 3 + 1] = v2[i].y - v1[i].y;
		vectorDiff[MONSTER_IDLE_3_TO_IDLE_1][i * 3 + 2] = v2[i].z - v1[i].z;

		// Idle 1 to Idle 3
		vectorDiff[MONSTER_IDLE_1_TO_IDLE_3][i * 3 + 0] = v1[i].x - v2[i].x;
		vectorDiff[MONSTER_IDLE_1_TO_IDLE_3][i * 3 + 1] = v1[i].y - v2[i].y;
		vectorDiff[MONSTER_IDLE_1_TO_IDLE_3][i * 3 + 2] = v1[i].z - v2[i].z;

		// To Run 1
		vectorDiff[MONSTER_IDLE_3_TO_RUN_1][i * 3 + 0] = v3[i].x - v1[i].x;
		vectorDiff[MONSTER_IDLE_3_TO_RUN_1][i * 3 + 1] = v3[i].y - v1[i].y;
		vectorDiff[MONSTER_IDLE_3_TO_RUN_1][i * 3 + 2] = v3[i].z - v1[i].z;
	}

	// Run 1
	size = monsterModels[MONSTER_RUN_1].Size();
	v1 = monsterModels[MONSTER_RUN_1].GetVertices();
	v2 = monsterModels[MONSTER_RUN_15].GetVertices();
	v3 = monsterModels[MONSTER_IDLE_1].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 15
		vectorDiff[MONSTER_RUN_1_TO_RUN_15][i * 3 + 0] = v2[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_1_TO_RUN_15][i * 3 + 1] = v2[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_1_TO_RUN_15][i * 3 + 2] = v2[i].z - v1[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_1_TO_IDLE_1][i * 3 + 0] = v3[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_1_TO_IDLE_1][i * 3 + 1] = v3[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_1_TO_IDLE_1][i * 3 + 2] = v3[i].z - v1[i].z;
	}

	// Run 15
	size = monsterModels[MONSTER_RUN_1].Size();
	v1 = monsterModels[MONSTER_RUN_2].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 2
		vectorDiff[MONSTER_RUN_15_TO_RUN_2][i * 3 + 0] = v1[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_15_TO_RUN_2][i * 3 + 1] = v1[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_15_TO_RUN_2][i * 3 + 2] = v1[i].z - v2[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_15_TO_IDLE_1][i * 3 + 0] = v3[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_15_TO_IDLE_1][i * 3 + 1] = v3[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_15_TO_IDLE_1][i * 3 + 2] = v3[i].z - v2[i].z;
	}

	// Run 2
	size = monsterModels[MONSTER_RUN_2].Size();
	v2 = monsterModels[MONSTER_RUN_25].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 25
		vectorDiff[MONSTER_RUN_2_TO_RUN_25][i * 3 + 0] = v2[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_2_TO_RUN_25][i * 3 + 1] = v2[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_2_TO_RUN_25][i * 3 + 2] = v2[i].z - v1[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_2_TO_IDLE_1][i * 3 + 0] = v3[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_2_TO_IDLE_1][i * 3 + 1] = v3[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_2_TO_IDLE_1][i * 3 + 2] = v3[i].z - v1[i].z;
	}

	// Run 25
	size = monsterModels[MONSTER_RUN_25].Size();
	v1 = monsterModels[MONSTER_RUN_3].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 3
		vectorDiff[MONSTER_RUN_25_TO_RUN_3][i * 3 + 0] = v1[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_25_TO_RUN_3][i * 3 + 1] = v1[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_25_TO_RUN_3][i * 3 + 2] = v1[i].z - v2[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_25_TO_IDLE_1][i * 3 + 0] = v3[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_25_TO_IDLE_1][i * 3 + 1] = v3[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_25_TO_IDLE_1][i * 3 + 2] = v3[i].z - v2[i].z;
	}

	// Run 3
	size = monsterModels[MONSTER_RUN_3].Size();
	v2 = monsterModels[MONSTER_RUN_35].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 35
		vectorDiff[MONSTER_RUN_3_TO_RUN_35][i * 3 + 0] = v2[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_3_TO_RUN_35][i * 3 + 1] = v2[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_3_TO_RUN_35][i * 3 + 2] = v2[i].z - v1[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_3_TO_IDLE_1][i * 3 + 0] = v3[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_3_TO_IDLE_1][i * 3 + 1] = v3[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_3_TO_IDLE_1][i * 3 + 2] = v3[i].z - v1[i].z;
	}

	// Run 35
	size = monsterModels[MONSTER_RUN_35].Size();
	v1 = monsterModels[MONSTER_RUN_4].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 4
		vectorDiff[MONSTER_RUN_35_TO_RUN_4][i * 3 + 0] = v1[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_35_TO_RUN_4][i * 3 + 1] = v1[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_35_TO_RUN_4][i * 3 + 2] = v1[i].z - v2[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_35_TO_IDLE_1][i * 3 + 0] = v3[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_35_TO_IDLE_1][i * 3 + 1] = v3[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_35_TO_IDLE_1][i * 3 + 2] = v3[i].z - v2[i].z;
	}

	// Run 4
	size = monsterModels[MONSTER_RUN_4].Size();
	v2 = monsterModels[MONSTER_RUN_45].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 45
		vectorDiff[MONSTER_RUN_4_TO_RUN_45][i * 3 + 0] = v2[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_4_TO_RUN_45][i * 3 + 1] = v2[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_4_TO_RUN_45][i * 3 + 2] = v2[i].z - v1[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_4_TO_IDLE_1][i * 3 + 0] = v3[i].x - v1[i].x;
		vectorDiff[MONSTER_RUN_4_TO_IDLE_1][i * 3 + 1] = v3[i].y - v1[i].y;
		vectorDiff[MONSTER_RUN_4_TO_IDLE_1][i * 3 + 2] = v3[i].z - v1[i].z;
	}

	// Run 45
	size = monsterModels[MONSTER_RUN_35].Size();
	v1 = monsterModels[MONSTER_RUN_1].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		// To Run 45
		vectorDiff[MONSTER_RUN_45_TO_RUN_1][i * 3 + 0] = v1[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_45_TO_RUN_1][i * 3 + 1] = v1[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_45_TO_RUN_1][i * 3 + 2] = v1[i].z - v2[i].z;

		// To Idle 1
		vectorDiff[MONSTER_RUN_45_TO_IDLE_1][i * 3 + 0] = v3[i].x - v2[i].x;
		vectorDiff[MONSTER_RUN_45_TO_IDLE_1][i * 3 + 1] = v3[i].y - v2[i].y;
		vectorDiff[MONSTER_RUN_45_TO_IDLE_1][i * 3 + 2] = v3[i].z - v2[i].z;
	}
}

float lastFraction = 0.0f;
vector<int> animationSequence;

void AnimateMonster(float Duration, Shader Shader)
{
	float time = glfwGetTime();
	float fraction = GetTimeFraction(time, Duration);

	int size = monsterModels[currentAnimation].Size();

	if (vectorResult == nullptr)
	{
		vectorResult = static_cast<GLfloat*>(malloc(sizeof(GLfloat) * 3 * size));
	}

	if (lastFraction > fraction)
	{
		// Go to next animation, since we looped
		currentAnimation = nextAnimation;
		nextAnimation = (nextAnimation + 1) % animationSequence.size();
		if (nextAnimation == 0)
		{
			++nextAnimation;
		}
	}

	int animNum = animationSequence[currentAnimation];
	int modelIndex = monsterAnimModelIndex[animNum];
	//cout << modelIndex << endl;
	vector<glm::vec3> vertices = monsterModels[modelIndex].GetVertices();
	for (int i = 0; i < size; ++i)
	{
		vectorResult[i * 3 + 0] = vertices[i].x + fraction * vectorDiff[animationSequence[nextAnimation]][i * 3 + 0];
		vectorResult[i * 3 + 1] = vertices[i].y + fraction * vectorDiff[animationSequence[nextAnimation]][i * 3 + 1];
		vectorResult[i * 3 + 2] = vertices[i].z + fraction * vectorDiff[animationSequence[nextAnimation]][i * 3 + 2];
	}

	// Now draw the updated model
	currentModel->UpdateVertices(vectorResult);
	currentModel->Draw(Shader);

	lastFraction = fraction;
}

int main(int argc, const char * argv[])
{
	srand(time(nullptr));

	// Initialize GLFW
	if (glfwInit())
	{
		// Set the GLFW Error Callback Function
		glfwSetErrorCallback(error_callback);

		// Set Minimum Versions
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

		if (window)
		{
			glfwSetKeyCallback(window, key_callback);
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetMouseButtonCallback(window, mouse_button_callback);
			glfwSetScrollCallback(window, scroll_callback);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glfwMakeContextCurrent(window);

			glewExperimental = GL_TRUE;
			if (glewInit() == GLEW_OK)
			{
				cout << "OpenGL version supported by this platform " << glGetString(GL_VERSION) << endl;
				const char* version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
				cout << "GLSL version supported by this platform " << version << endl;

				static const GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
				static const GLfloat clearDepth[] = { 1.0f, 1.0f, 1.0f, 1.0f };

				glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
				
				glEnable(GL_DEPTH_TEST);
				
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glEnable(GL_CULL_FACE);

				Shader shader("data/shaders/texturedMultiLights.vs", "data/shaders/texturedMultiLights.fs");
				Shader singleTextureShader("data/shaders/texturedMultiLights.vs", "data/shaders/texturedMultiLights.fs");
				
				shader.Use();

				// Set up lights
				mouseFlashLight = SpotLight(camera.Position, // Light Position
								glm::vec3(1.0f, 1.0f, 1.0f), // Color, using fire values for testing (1.0f, 0.5765f, 0.1508f)
								camera.Front,
								0.4f, // Ambient Intensity
								1.0f, // Diffuse Intensity
								5.0f, // Specular Intensity
								1.0f, 0.09f, 0.032f, // Attenuation Coefficients
								glm::cos(glm::radians(15.0f)), glm::cos(glm::radians(20.0f))); // CutOff Values

				PointLight sun = PointLight(glm::vec3(0.0f, 10.0f, 0.0f),
											glm::vec3(1.0f, 0.5765f, 0.1508f),
											0.4f,
											1.0f,
											1.0f,
											1.0f, 0.09f, 0.032f);

				// Set up models
				BrickFloor = Model("data/models/brickfloor/brickfloor.obj", true);
				//Model playerBody("data/models/nanosuit/nanosuit.obj", true);
				vector<Model> gems;
				for (int i = 0; i < 20; ++i)
				{
					Model gem("data/models/gem/gem.obj", true);
					glm::mat4 matrix;
					float randX = -5.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10);
					float randZ = -5.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10);
					glm::vec3 randomDir = glm::vec3(randX, 0.0f, randZ);
					matrix = glm::translate(randomDir);
					gem.SetMatrix(matrix);
					gems.push_back(gem);
				}

				shader.AddLight(&mouseFlashLight);

				glUseProgram(0);

				singleTextureShader.Use();

				// Set up floor
				//singleTextureShader.AddLight(&mouseFlashLight);
				singleTextureShader.AddLight(&sun);
				
				glUseProgram(0);

				Shader monsterShader("data/shaders/texturedMultiLights.vs", "data/shaders/texturedMultiLights.fs");
				monsterShader.Use();

				currentModel = new Model("data/models/flesh_creature/flesh_creature_idle1.obj", true);
				monsterModels = new Model[11];
				monsterModels[MONSTER_IDLE_1] = Model("data/models/flesh_creature/flesh_creature_idle1.obj", true);
				monsterModels[MONSTER_IDLE_2] = Model("data/models/flesh_creature/flesh_creature_idle2.obj", true);
				monsterModels[MONSTER_IDLE_3] = Model("data/models/flesh_creature/flesh_creature_idle3.obj", true);
				monsterModels[MONSTER_RUN_1] = Model("data/models/flesh_creature/flesh_creature_run1.obj", true);
				monsterModels[MONSTER_RUN_15] = Model("data/models/flesh_creature/flesh_creature_run15.obj", true);
				monsterModels[MONSTER_RUN_2] = Model("data/models/flesh_creature/flesh_creature_run2.obj", true);
				monsterModels[MONSTER_RUN_25] = Model("data/models/flesh_creature/flesh_creature_run25.obj", true);
				monsterModels[MONSTER_RUN_3] = Model("data/models/flesh_creature/flesh_creature_run3.obj", true);
				monsterModels[MONSTER_RUN_35] = Model("data/models/flesh_creature/flesh_creature_run35.obj", true);
				monsterModels[MONSTER_RUN_4] = Model("data/models/flesh_creature/flesh_creature_run4.obj", true);
				monsterModels[MONSTER_RUN_45] = Model("data/models/flesh_creature/flesh_creature_run45.obj", true);

				glm::mat4 monsterMat = glm::mat4();
				monsterMat = glm::scale(monsterMat, glm::vec3(0.3f, 0.3f, 0.3f));
				monsterMat = glm::translate(monsterMat, glm::vec3(5.0f, -1.7f, 5.0f));

				InitMorphing();
				animationSequence = { 0, MONSTER_IDLE_1_TO_IDLE_2, MONSTER_IDLE_2_TO_IDLE_1, MONSTER_IDLE_1_TO_IDLE_3, MONSTER_IDLE_3_TO_IDLE_1 };
				//animationSequence = { 0, MONSTER_RUN_1_TO_RUN_15, MONSTER_RUN_15_TO_RUN_2, MONSTER_RUN_2_TO_RUN_25, MONSTER_RUN_25_TO_RUN_3, MONSTER_RUN_3_TO_RUN_35, MONSTER_RUN_35_TO_RUN_4, MONSTER_RUN_4_TO_RUN_45, MONSTER_RUN_45_TO_RUN_1 };
				currentAnimation = 1;
				nextAnimation = 2;

				monsterShader.AddLight(&mouseFlashLight);

				glUseProgram(0);

				// CubeMap
				GLfloat skyboxVertices[] = {
					// Positions          
					-25.0f,  25.0f, -25.0f,
					-25.0f, -25.0f, -25.0f,
					25.0f, -25.0f, -25.0f,
					25.0f, -25.0f, -25.0f,
					25.0f,  25.0f, -25.0f,
					-25.0f,  25.0f, -25.0f,

					-25.0f, -25.0f,  25.0f,
					-25.0f, -25.0f, -25.0f,
					-25.0f,  25.0f, -25.0f,
					-25.0f,  25.0f, -25.0f,
					-25.0f,  25.0f,  25.0f,
					-25.0f, -25.0f,  25.0f,

					25.0f, -25.0f, -25.0f,
					25.0f, -25.0f,  25.0f,
					25.0f,  25.0f,  25.0f,
					25.0f,  25.0f,  25.0f,
					25.0f,  25.0f, -25.0f,
					25.0f, -25.0f, -25.0f,

					-25.0f, -25.0f,  25.0f,
					-25.0f,  25.0f,  25.0f,
					25.0f,  25.0f,  25.0f,
					25.0f,  25.0f,  25.0f,
					25.0f, -25.0f,  25.0f,
					-25.0f, -25.0f,  25.0f,

					-25.0f,  25.0f, -25.0f,
					25.0f,  25.0f, -25.0f,
					25.0f,  25.0f,  25.0f,
					25.0f,  25.0f,  25.0f,
					-25.0f,  25.0f,  25.0f,
					-25.0f,  25.0f, -25.0f,

					-25.0f, -25.0f, -25.0f,
					-25.0f, -25.0f,  25.0f,
					25.0f, -25.0f, -25.0f,
					25.0f, -25.0f, -25.0f,
					-25.0f, -25.0f,  25.0f,
					25.0f, -25.0f,  25.0f
				};

				Shader skyboxShader("data/shaders/cubemap.vs", "data/shaders/cubemap.fs");

				skyboxShader.Use();

				GLuint skyboxVAO;
				GLuint skyboxVBO;

				glGenVertexArrays(1, &skyboxVAO);
				glBindVertexArray(skyboxVAO);

				glGenBuffers(1, &skyboxVBO);
				glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 & sizeof(GLfloat), static_cast<GLvoid*>(0));

				glBindVertexArray(0);

				vector<const GLchar*> skyboxFaces;
				skyboxFaces.push_back("data/textures/starfield_rt.jpg");
				skyboxFaces.push_back("data/textures/starfield_lf.jpg");
				skyboxFaces.push_back("data/textures/starfield_up.jpg");
				skyboxFaces.push_back("data/textures/starfield_dn.jpg");
				skyboxFaces.push_back("data/textures/starfield_bk.jpg");
				skyboxFaces.push_back("data/textures/starfield_ft.jpg");
				GLuint skyboxTexture = LoadCubeMap(skyboxFaces);

				glUseProgram(0);

				// Draw wireframe model
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				int cameraUniformLocation;
				int projectionUniformLocation;
				int viewUniformLocation;
				int modelUniformLocation;

				glm::mat4 projection;
				glm::mat4 view;
				glm::mat4 model;

				int points = 0;

				while(!glfwWindowShouldClose(window))
				{
					GLfloat currentFrame = glfwGetTime();
					deltaTime = currentFrame - lastFrame;
					lastFrame = currentFrame;

					glfwPollEvents();
					DoMovement();

					glClearBufferfv(GL_COLOR , 0, clearColor);
					glClearBufferfv(GL_DEPTH , 0, clearDepth);

					projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
					view = camera.GetViewMatrix();
					model = glm::mat4();

					if (mouseFlashLight.Enabled)
					{
						for (auto gem = gems.begin(); gem != gems.end(); ++gem)
						{
							glm::mat4 gemMat = (*gem).GetMatrix();
							glm::vec3 gemPos = glm::vec3(gemMat[3]);
							float distance = glm::length(camera.Position - gemPos);
							if (distance < 1.0f)
							{
								glm::mat4 matrix;
								float randX = -5.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10);
								float randZ = -5.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10);
								glm::vec3 randomDir = glm::vec3(randX, 0.0f, randZ);
								matrix = glm::translate(randomDir);
								(*gem).SetMatrix(matrix);
								cout << "Points: " << ++points << endl;
							}
						}
					}

					skyboxShader.Use();

					projectionUniformLocation = glGetUniformLocation(skyboxShader.GetProgram(), "projection");
					viewUniformLocation = glGetUniformLocation(skyboxShader.GetProgram(), "view");

					glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
					glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

					glDepthMask(GL_FALSE);

					glBindVertexArray(skyboxVAO);
					glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glBindVertexArray(0);

					glDepthMask(GL_TRUE);

					glUseProgram(0);


					singleTextureShader.Use();

					cameraUniformLocation = glGetUniformLocation(singleTextureShader.GetProgram(), "camera");
					projectionUniformLocation = glGetUniformLocation(singleTextureShader.GetProgram(), "projection");
					viewUniformLocation = glGetUniformLocation(singleTextureShader.GetProgram(), "view");
					modelUniformLocation = glGetUniformLocation(singleTextureShader.GetProgram(), "model");

					glUniform3fv(cameraUniformLocation, 1, glm::value_ptr(camera.Position));
					glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
					glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

					model = glm::translate(glm::vec3(0.0f, -0.5f, 0.0f));
					glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
						
					// Set up the light positions
					mouseFlashLight.Position = glm::vec4(camera.Position, 2.0f);
					mouseFlashLight.Direction = camera.Front;
					sun.Position = glm::vec4(camera.Position, 1.0f);

					BrickFloor.Draw(singleTextureShader);

					glBindVertexArray(0);
					glUseProgram(0);

					shader.Use();

					cameraUniformLocation = glGetUniformLocation(shader.GetProgram(), "camera");
					projectionUniformLocation = glGetUniformLocation(shader.GetProgram(), "projection");
					viewUniformLocation = glGetUniformLocation(shader.GetProgram(), "view");

					glUniform3fv(cameraUniformLocation, 1, glm::value_ptr(camera.Position));
					glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
					glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

					for (int i = 0; i < gems.size(); ++i)
					{
						model = gems[i].GetMatrix();
						model *= glm::rotate(static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
						model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

						modelUniformLocation = glGetUniformLocation(shader.GetProgram(), "model");
						glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));

						int dimensionUniformLocation = glGetUniformLocation(monsterShader.GetProgram(), "alternateDimension");
						glUniform1i(dimensionUniformLocation, true);

						if (mouseFlashLight.Enabled)
						{
							gems[i].Draw(shader);
						}
					}

					glUseProgram(0);

					monsterShader.Use();

					cameraUniformLocation = glGetUniformLocation(shader.GetProgram(), "camera");
					projectionUniformLocation = glGetUniformLocation(monsterShader.GetProgram(), "projection");
					viewUniformLocation = glGetUniformLocation(monsterShader.GetProgram(), "view");

					glUniform3fv(cameraUniformLocation, 1, glm::value_ptr(camera.Position));
					glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
					glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

					float monsterYaw;
					float monsterPitch;
					float monsterRoll;
					monsterPosition = glm::vec3(monsterMat[3]);

					GetRotation(monsterMat, monsterYaw, monsterPitch, monsterRoll);
					monsterYaw += glm::radians(180.0f);

					glm::vec3 monsterHeading = glm::normalize(glm::vec3(glm::cos(monsterYaw), 0.0f, -glm::sin(monsterYaw)));
					glm::vec3 normalizedCamera = glm::vec3(camera.Position.x, 0.0f, camera.Position.z);
					glm::vec3 normalizedMonster = glm::vec3(monsterPosition.x, 0.0f, monsterPosition.z);
					glm::vec3 normalizedCameraMonster = glm::normalize(normalizedCamera - normalizedMonster);

					float dotProduct = glm::min(glm::dot(normalizedCameraMonster, monsterHeading), 1.0f);
					float camToMonsterAngle = acosf(dotProduct);
					if (mouseFlashLight.Enabled)
					{
						if (glm::degrees(camToMonsterAngle) < monsterFOV)
						{
							if (!monsterCanSeeYou)
							{
								int transitionAnim = 0;
								switch(animationSequence[currentAnimation])
								{
									case MONSTER_IDLE_1_TO_IDLE_2:
										transitionAnim = MONSTER_IDLE_1_TO_RUN_1;
										break;
									case MONSTER_IDLE_2_TO_IDLE_1:
										transitionAnim = MONSTER_IDLE_2_TO_RUN_1;
										break;
									case MONSTER_IDLE_1_TO_IDLE_3:
										transitionAnim = MONSTER_IDLE_1_TO_RUN_1;
										break;
									case MONSTER_IDLE_3_TO_IDLE_1:
										transitionAnim = MONSTER_IDLE_3_TO_RUN_1;
										break;
								}
								animationSequence = { 0, MONSTER_RUN_1_TO_RUN_15, MONSTER_RUN_15_TO_RUN_2, MONSTER_RUN_2_TO_RUN_25, MONSTER_RUN_25_TO_RUN_3, MONSTER_RUN_3_TO_RUN_35, MONSTER_RUN_35_TO_RUN_4, MONSTER_RUN_4_TO_RUN_45, MONSTER_RUN_45_TO_RUN_1 };
								currentAnimation = 0;
								nextAnimation = 1;
							}
							monsterCanSeeYou = true;
						}
						else
						{
							if (monsterCanSeeYou)
							{
								int transitionAnim = 0;
								switch(animationSequence[currentAnimation])
								{
									case MONSTER_RUN_1_TO_RUN_15:
										transitionAnim = MONSTER_RUN_1_TO_IDLE_1;
										break;
									case MONSTER_RUN_15_TO_RUN_2:
										transitionAnim = MONSTER_RUN_15_TO_IDLE_1;
										break;
									case MONSTER_RUN_2_TO_RUN_25:
										transitionAnim = MONSTER_RUN_2_TO_IDLE_1;
										break;
									case MONSTER_RUN_25_TO_RUN_3:
										transitionAnim = MONSTER_RUN_25_TO_IDLE_1;
										break;
									case MONSTER_RUN_3_TO_RUN_35:
										transitionAnim = MONSTER_RUN_3_TO_IDLE_1;
										break;
									case MONSTER_RUN_35_TO_RUN_4:
										transitionAnim = MONSTER_RUN_35_TO_IDLE_1;
										break;
									case MONSTER_RUN_4_TO_RUN_45:
										transitionAnim = MONSTER_RUN_4_TO_IDLE_1;
										break;
									case MONSTER_RUN_45_TO_RUN_1:
										transitionAnim = MONSTER_RUN_45_TO_IDLE_1;
										break;
								}
								animationSequence = { 0, MONSTER_IDLE_1_TO_IDLE_2, MONSTER_IDLE_2_TO_IDLE_1, MONSTER_IDLE_1_TO_IDLE_3, MONSTER_IDLE_3_TO_IDLE_1 };
								currentAnimation = 0;
								nextAnimation = 1;
							}
							monsterCanSeeYou = false;
						}
					}

					if (monsterCanSeeYou)
					{
						glm::vec3 c = glm::cross(camera.Position, monsterHeading);
						float rotAngle = -glm::sign(c.y) * camToMonsterAngle;
						monsterMat = glm::rotate(monsterMat, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
						GetRotation(monsterMat, monsterYaw, monsterPitch, monsterRoll);
						monsterYaw += glm::radians(180.0f);
						monsterHeading = glm::normalize(glm::vec3(glm::cos(monsterYaw), 0.0f, -glm::sin(monsterYaw)));
						glm::mat4 trans = glm::translate(monsterHeading * 0.005f);
						monsterMat[3][0] += trans[3][0];
						monsterMat[3][1] += trans[3][1];
						monsterMat[3][2] += trans[3][2];
					}
					else
					{
						monsterMat = glm::rotate(monsterMat, glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
					}

					modelUniformLocation = glGetUniformLocation(monsterShader.GetProgram(), "model");
					glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(monsterMat));

					int dimensionUniformLocation = glGetUniformLocation(monsterShader.GetProgram(), "alternateDimension");
					glUniform1i(dimensionUniformLocation, true);

					glBindTexture(GL_TEXTURE0, 0);
					if (monsterCanSeeYou)
					{
						AnimateMonster(0.1f, monsterShader);
					}
					else
					{
						AnimateMonster(1.0f, monsterShader);
					}

					// Check death condition

					glm::vec3 posCam = glm::vec3(camera.Position.x, 0.0f, camera.Position.z);
					glm::vec3 posMon = glm::vec3(monsterPosition.x, 0.0f, monsterPosition.z);
					float len = glm::length(posCam - posMon);
					if (glm::length(posCam - posMon) < 1.0f)
					{
						cout << "You're dead!" << endl;
					}

					glUseProgram(0);

					glfwSwapBuffers(window);
				}
			}
			else
			{
				cerr << "GLEW Initialization Failed" << endl;
			}
		}
		else
		{
			cerr << "GLFW Window Creation Failed!" << endl;
		}
	}
	else
	{
		cerr << "GLFW Initialization Failed!" << endl;
	}

	//glfwTerminate();

	return EXIT_SUCCESS;
}

