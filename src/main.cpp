/**
 * OpenGL Template By: 	Andrew Robert Owens
 * Modifications By: 	Shannon TJ
 * Date:				March 21, 2017
 * Course:				CPSC 587 Computer Animation
 * 
 * University of Calgary
 *
 *
 * This program displays four mass-spring simulations:
 * 1. Single spring
 * 2. Chain pendulum
 * 3. Jello cube
 * 4. Hanging cloth
 * 
 */

#include <iostream>
#include <cmath>
#include <chrono>
#include <limits>
#include <vector>
#include <array>
#include <cstring>

#include "glm/glm.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "ShaderTools.h"
#include "Vec3f.h"
#include "Mat4f.h"
#include "OpenGLMatrixTools.h"
#include "Camera.h"

using namespace std;
using namespace glm;

//==================== GLOBAL VARIABLES ====================//
/*	Put here for simplicity. Feel free to restructure into
*	appropriate classes or abstractions.
*/

// Drawing Program
GLuint basicProgramID;

// Data needed for Quad
GLuint vaoID;
GLuint vertBufferID;
Mat4f M;

// Data needed for Line 
GLuint line_vaoID;
GLuint line_vertBufferID;
Mat4f line_M;

// Only one camera so only one view and perspective matrix are needed.
Mat4f V;
Mat4f P;

// Only one thing is rendered at a time, so only need one MVP
// When drawing different objects, update M and MVP = M * V * P
Mat4f MVP;

// Camera and viewing Stuff
Camera camera;
int g_moveUpDown = 0;
int g_moveLeftRight = 0;
int g_moveBackForward = 0;
int g_rotateLeftRight = 0;
int g_rotateUpDown = 0;
int g_rotateRoll = 0;
float g_rotationSpeed = 0.015625;
float g_panningSpeed = 0.25;
bool g_cursorLocked;
float g_cursorX, g_cursorY;

bool g_play = false;

int WIN_WIDTH = 800, WIN_HEIGHT = 800;
int FB_WIDTH = 800, FB_HEIGHT = 600;
float WIN_FOV = 60;
float WIN_NEAR = 0.01;
float WIN_FAR = 1000;

int numMass = 2;
int numSpring = 1;

float damping = 0.8f;
float timestep = 0.01f;

bool sim1 = true;
bool sim2 = false;
bool sim3 = false;
bool sim4 = false;

std::vector<Vec3f> verts;
std::vector<Vec3f> verts2;

//==================== FUNCTION DECLARATIONS ====================//
void displayFunc();
void resizeFunc();
void init();
void generateIDs();
void deleteIDs();
void setupVAO();
void reloadProjectionMatrix();
void loadModelViewMatrix();
void setupModelViewProjectionTransform();

void windowSetSizeFunc();
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowSetSizeFunc(GLFWwindow *window, int width, int height);
void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height);
void windowMouseButtonFunc(GLFWwindow *window, int button, int action,
                           int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void moveCamera();
void reloadMVPUniform();
void reloadColorUniform(float r, float g, float b);
std::string GL_ERROR();
int main(int, char **);

//==================== FUNCTION DEFINITIONS ====================//
struct Mass
{
	float mass;
	bool fixedPoint;
	
	vec3 position;
	vec3 velocity;
	vec3 acc;
};

struct Spring
{
	Mass *a, *b;
	float stiffness;
	float restLength;
};

//Masses and Spring for sim1
Mass m;
Spring s;

//Masses and Springs for sim2
Mass mChain;
Spring sChain;

//Masses and springs for sim3
Mass mCube;
Spring sCube;

//Masses and springs for sim4
Mass mCloth;
Spring sCloth;

//Arrays
Spring springs1 [1];
Mass masses1 [2];

Spring springs2 [3];
Mass masses2 [4];

Spring springs3 [126];
Mass masses3 [27];  

Spring springs4[89];
Mass masses4[30];

//Get length between masses
float getLength(Mass *a, Mass *b)
{
	float length1 = ((b->position.x)-(a->position.x))*((b->position.x)-(a->position.x));
	float length2 = ((b->position.y)-(a->position.y))*((b->position.y)-(a->position.y));
	float length3 = ((b->position.z)-(a->position.z))*((b->position.z)-(a->position.z));	
	float springLength = sqrt(length1 + length2 + length3);
	
	return springLength;
}

//Initialize masses
Mass initMass(Mass ms, float weight, bool fix, vec3 pos)
{
	ms.mass = weight;
	ms.fixedPoint = fix;
	ms.position = pos;
	ms.velocity = vec3(0,0,0);
	ms.acc = vec3(0,0,0);
	
	return ms;
} 

//Initialize springs
Spring initSpring(Spring ss, Mass *a, Mass *b, float k, float rLen)
{
	ss.a = a;
	ss.b = b;
	ss.stiffness = k;
	ss.restLength = rLen;
	
	return ss;
}

//Single spring
void initSim1()
{
	numMass = 2;
	numSpring = 1;
	
	m = initMass(m, 1.f, true, vec3(0,3.5f,0));
	masses1[0] = m;
	
	m = initMass(m, 1.f, false, vec3(2.f,2.f,0));	
	masses1[1] = m;
	
	s = initSpring(s, &masses1[0], &masses1[1], 25.f, 1.f);	
	springs1[0] = s;
	
	sim1 = true;
	sim2 = false;
    sim3 = false;
	sim4 = false;
}

//Chain pendulum
void initSim2()
{
	numMass = 4;
	numSpring = 3;
	
	mChain = initMass(mChain, 1.f, true, vec3(0,3.5f,0));	
	masses2[0] = mChain;
	
	mChain = initMass(mChain, 0.5f, false, vec3(0,3.f,0));		
	masses2[1] = mChain;
	
	mChain = initMass(mChain, 0.5f, false, vec3(0.5f,3.5f,0));			
	masses2[2] = mChain;
	
	mChain = initMass(mChain, 1.5f, false, vec3(1.f,3.5f,0));			
	masses2[3] = mChain;

	sChain = initSpring(sChain, &masses2[0], &masses2[1], 25.f, 1.f);		
	springs2[0] = sChain;
	
	sChain = initSpring(sChain, &masses2[1], &masses2[2], 25.f, 1.f);		
    springs2[1] = sChain;
	
	sChain = initSpring(sChain, &masses2[2], &masses2[3], 25.f, 1.f);		
	springs2[2] = sChain;
	
	sim1 = false;
	sim2 = true;
    sim3 = false;
	sim4 = false;
}

//Jello cube
void initSim3()
{
	numMass = 27;
	numSpring = 126;
	
	float x = -.9f;
	float y = 3.5f;
	float z = 0;
	float k = 25;
	float rLen = 0;
	
	//Initialize the masses
	for(int i = 0; i < numMass; i++)
	{		
		mCube = initMass(mCube, 0.3f, false, vec3(x,y,z));
		masses3[i] = mCube;
		x = x + 1.f;
		
		if(i == 2 || i == 5 || i == 11 || i == 14 || i == 20 || i == 23)
		{
			x = -.9f;
			y = y - 1.f;
		}
		
		else if(i == 8 || i == 17)
		{
			x = -.9f;
			y = 3.5f;
			z = z - 1.f;
		}
	}
	
	//Initialize the springs
	int m = 0;
	for(int i = 0; i < numMass; i++)
	{
		for(int j = 0; j < numMass; j++)
		{
			rLen = getLength(&masses3[i], &masses3[j]);
			
			//x axis
			if(masses3[i].position.x - masses3[j].position.x == 1.f && masses3[i].position.y == masses3[j].position.y)
			{
				if(masses3[i].position.z == masses3[j].position.z)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;
				}
			}
						
			//y axis
			if(masses3[i].position.y - masses3[j].position.y == 1.f && masses3[i].position.z == masses3[j].position.z)
			{
				if(masses3[i].position.x == masses3[j].position.x)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;
				}
			}	
			
			//z axis
			if(masses3[i].position.z - masses3[j].position.z == 1.f && masses3[i].position.y == masses3[j].position.y)
			{
				if(masses3[i].position.x == masses3[j].position.x)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;	
				}
			}	
			
			//Side crosses
			if(masses3[i].position.z - masses3[j].position.z == 1.f && masses3[i].position.y - masses3[j].position.y == 1.f)
			{
				if(masses3[i].position.x == masses3[j].position.x)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;		
				}
			}			
			
			if(masses3[i].position.z - masses3[j].position.z == 1.f && masses3[i].position.y - masses3[j].position.y == -1.f)
			{
				if(masses3[i].position.x == masses3[j].position.x)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;	
				}
			}		
		
			//Horizontal crosses
			if(masses3[i].position.z - masses3[j].position.z == 1.f && masses3[i].position.x - masses3[j].position.x == 1.f)
			{
				if(masses3[i].position.y == masses3[j].position.y)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;	
				}
			}			
			
			if(masses3[i].position.z - masses3[j].position.z == 1.f && masses3[i].position.x - masses3[j].position.x == -1.f)
			{
				if(masses3[i].position.y == masses3[j].position.y)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;	
				}
			}			
			
			//Front and back crosses
			if(masses3[i].position.x - masses3[j].position.x == 1.f && masses3[i].position.y - masses3[j].position.y == 1.f)
			{
				if(masses3[i].position.z == masses3[j].position.z)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;	
				}
			}			
			
			if(masses3[i].position.x - masses3[j].position.x == 1.f && masses3[i].position.y - masses3[j].position.y == -1.f)
			{
				if(masses3[i].position.z == masses3[j].position.z)
				{
					sCube = initSpring(sCube, &masses3[i], &masses3[j], k, rLen);
					springs3[m] = sCube;
					m = m + 1;		
				}
			}				
		}	
	}
	
	sim1 = false;
	sim2 = false;
    sim3 = true;
	sim4 = false;
}

//Hanging cloth
void initSim4()
{
	numMass = 30;
	numSpring = 89;
	
	float x = -2.f;
	float y = 3.5f;
	float z = 0;
	float k = 25;
	float rLen = 0;
	
	//Initialize the masses
	for(int i = 0; i < numMass; i++)
	{	
		if(i == 0 || i == 2 || i == 4)
			mCloth = initMass(mCloth, 0.5f, true, vec3(x,y,z));
		else
			mCloth = initMass(mCloth, 0.5f, false, vec3(x,y,z));
		
		masses4[i] = mCloth;	
		x = x + 1.f;
		
		if(i == 4 || i == 9 || i == 14 || i == 19 || i == 24)
		{
			x = -2.f;
			z = z - 1.f;
		}
	}
	
	//Initialize the springs
	int m = 0;
	for(int i = 0; i < numMass; i++)
	{
		for(int j = 0; j < numMass; j++)
		{
			rLen = getLength(&masses4[i], &masses4[j]);
			
			//Vertical lines
			if(masses4[i].position.x == masses4[j].position.x && masses4[i].position.z - masses4[j].position.z == 1.f)
			{
					sCloth = initSpring(sCloth, &masses4[i], &masses4[j], k, rLen);
					springs4[m] = sCloth;
					m = m + 1;
			}
			
			//Horizontal lines
			if(masses4[i].position.z == masses4[j].position.z && masses4[i].position.x - masses4[j].position.x == 1.f)
			{
					sCloth = initSpring(sCloth, &masses4[i], &masses4[j], k, rLen);									
					springs4[m] = sCloth;
					m = m + 1;
			}
			
			//Crossed lines
			if(masses4[i].position.z - masses4[j].position.z == 1.f && masses4[i].position.x - masses4[j].position.x == 1.f)
			{
					sCloth = initSpring(sCloth, &masses4[i], &masses4[j], k, rLen);
					springs4[m] = sCloth;
					m = m + 1;
			}			
			
			if(masses4[i].position.z - masses4[j].position.z == 1.f && masses4[i].position.x - masses4[j].position.x == -1.f)
			{
					sCloth = initSpring(sCloth, &masses4[i], &masses4[j], k, rLen);
					springs4[m] = sCloth;
					m = m + 1;
			}		
		}
	}
	
	sim1 = false;
	sim2 = false;
    sim3 = false;
	sim4 = true;
}

void applyForces(Spring s, Mass *a, Mass *b)
{
	//Get current length of spring
	float springLength = getLength(a, b);
	vec3 unitAB = (b->position - a->position)/springLength;
	
	//hooke = -k(x-x0)AB
	vec3 hooke = (-s.stiffness)*(springLength - s.restLength)*unitAB;
	
	vec3 bAcc = hooke/(b->mass);
	vec3 aAcc = -bAcc;

	//Update acceleration for mass *a and *b
	b->acc += bAcc;
	a->acc += aAcc;
}	
	
void resolveForces(Mass *m)
{	
	//apply gravity and damping
	vec3 gravity = vec3(0.f,-9.81f,0.f);
	vec3 vDamping = ((-damping)*(m->velocity))/m->mass;
	
	//apply all accelerations
	m->acc = m->acc + gravity + vDamping;
		
		
	if(m->fixedPoint == false)
	{
		if(sim3 && m->position.y < -2.f)
		{
			m->velocity = m->velocity + m->acc*timestep;
			m->velocity = -m->velocity;
		}

		else
		{
			m->velocity = m->velocity + m->acc*timestep;
			m->position = m->position + m->velocity*timestep;
		}
	}
	
	m->acc = vec3(0,0,0);
}

void displayFunc() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our shader
  glUseProgram(basicProgramID);

  // ===== DRAW QUAD ====== //
  MVP = P * V * M;
  reloadMVPUniform();
  reloadColorUniform(0, 0, 1);

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(vaoID);
  // Draw Quads, start at vertex 0, draw 4 of them (for a quad)
  glDrawArrays(GL_TRIANGLES, 0, 6*numMass);

  // ==== DRAW LINE ===== //
  MVP = P * V * line_M;
  reloadMVPUniform();

  reloadColorUniform(0, 1, 1);

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(line_vaoID);
  // Draw lines
  glDrawArrays(GL_LINES, 0, 2*numSpring);
}

void animateQuad(Mass m) 
{
	 verts2.push_back(Vec3f(m.position.x-0.05f, m.position.y-0.05f, m.position.z));
	 verts2.push_back(Vec3f(m.position.x-0.05f, m.position.y+0.05f, m.position.z));
     verts2.push_back(Vec3f(m.position.x+0.05f, m.position.y-0.05f, m.position.z));
			  
	 verts2.push_back(Vec3f(m.position.x+0.05f, m.position.y+0.05f, m.position.z));
	 verts2.push_back(Vec3f(m.position.x-0.05f, m.position.y+0.05f, m.position.z));  
	 verts2.push_back(Vec3f(m.position.x+0.05f, m.position.y-0.05f, m.position.z));

	  glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
	  glBufferData(GL_ARRAY_BUFFER,
				   sizeof(Vec3f) * (6*numMass), // byte size of Vec3f, 4 of them
				   verts2.data(),      // pointer (Vec3f*) to contents of verts
				   GL_STATIC_DRAW);   // Usage pattern of GPU buffer*/
}

void animateSpring(Spring s)
{
	verts.push_back(Vec3f(s.a->position.x, s.a->position.y, s.a->position.z));
	verts.push_back(Vec3f(s.b->position.x, s.b->position.y, s.b->position.z));
	  
	 glBindBuffer(GL_ARRAY_BUFFER, line_vertBufferID);
	 glBufferData(GL_ARRAY_BUFFER,
				   sizeof(Vec3f) * (2*numSpring), // byte size of Vec3f, 4 of them
				   verts.data(),      // pointer (Vec3f*) to contents of verts
				   GL_STATIC_DRAW);   // Usage pattern of GPU buffer
}

void setupVAO() {
  glBindVertexArray(vaoID);

  glEnableVertexAttribArray(0); // match layout # in shader
  glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
  glVertexAttribPointer(0,        // attribute layout # above
                        3,        // # of components (ie XYZ )
                        GL_FLOAT, // type of components
                        GL_FALSE, // need to be normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
                        );

  glBindVertexArray(line_vaoID);

  glEnableVertexAttribArray(0); // match layout # in shader
  glBindBuffer(GL_ARRAY_BUFFER, line_vertBufferID);
  glVertexAttribPointer(0,        // attribute layout # above
                        3,        // # of components (ie XYZ )
                        GL_FLOAT, // type of components
                        GL_FALSE, // need to be normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
                        );

  glBindVertexArray(0); // reset to default
}

void reloadProjectionMatrix() {
	
  P = PerspectiveProjection(WIN_FOV, // FOV
                            static_cast<float>(WIN_WIDTH) /
                                WIN_HEIGHT, // Aspect
                            WIN_NEAR,       // near plane
                            WIN_FAR);       // far plane depth
}

void loadModelViewMatrix() {
  M = IdentityMatrix();
  line_M = IdentityMatrix();
  V = camera.lookatMatrix();
}

void reloadViewMatrix() { V = camera.lookatMatrix(); }

void setupModelViewProjectionTransform() {
  MVP = P * V * M; // transforms vertices from right to left (odd huh?)
}

void reloadMVPUniform() {
  GLint id = glGetUniformLocation(basicProgramID, "MVP");

  glUseProgram(basicProgramID);
  glUniformMatrix4fv(id,        // ID
                     1,         // only 1 matrix
                     GL_TRUE,   // transpose matrix, Mat4f is row major
                     MVP.data() // pointer to data in Mat4f
                     );
}

void reloadColorUniform(float r, float g, float b) {
  GLint id = glGetUniformLocation(basicProgramID, "inputColor");

  glUseProgram(basicProgramID);
  glUniform3f(id, // ID in basic_vs.glsl
              r, g, b);
}

void generateIDs() {
  // shader ID from OpenGL
  std::string vsSource = loadShaderStringfromFile("./shaders/basic_vs.glsl");
  std::string fsSource = loadShaderStringfromFile("./shaders/basic_fs.glsl");
  basicProgramID = CreateShaderProgram(vsSource, fsSource);

  // VAO and buffer IDs given from OpenGL
  glGenVertexArrays(1, &vaoID);
  glGenBuffers(1, &vertBufferID);
  glGenVertexArrays(1, &line_vaoID);
  glGenBuffers(1, &line_vertBufferID);
}

void deleteIDs() {
  glDeleteProgram(basicProgramID);

  glDeleteVertexArrays(1, &vaoID);
  glDeleteBuffers(1, &vertBufferID);
  glDeleteVertexArrays(1, &line_vaoID);
  glDeleteBuffers(1, &line_vertBufferID);
}

void init() {
  glEnable(GL_DEPTH_TEST);
  glPointSize(50);

  camera = Camera(Vec3f{0, 0, 7}, Vec3f{0, 0, -1}, Vec3f{0, 1, 0});

  // SETUP SHADERS, BUFFERS, VAOs

  generateIDs();
  setupVAO();

  loadModelViewMatrix();
  reloadProjectionMatrix();
  setupModelViewProjectionTransform();
  reloadMVPUniform();
}

int main(int argc, char **argv) {
		
  GLFWwindow *window;

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window =
      glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "CPSC 587 A3", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetWindowSizeCallback(window, windowSetSizeFunc);
  glfwSetFramebufferSizeCallback(window, windowSetFramebufferSizeFunc);
  glfwSetKeyCallback(window, windowKeyFunc);
  glfwSetCursorPosCallback(window, windowMouseMotionFunc);
  glfwSetMouseButtonCallback(window, windowMouseButtonFunc);

  glfwGetFramebufferSize(window, &WIN_WIDTH, &WIN_HEIGHT);

  // Initialize glad
  if (!gladLoadGL()) {
    std::cerr << "Failed to initialise GLAD" << std::endl;
    return -1;
  }

  std::cout << "GL Version: :" << glGetString(GL_VERSION) << std::endl;
  std::cout << GL_ERROR() << std::endl;

  init(); 
  initSim1();
  
  //Calculate spring/mass positions, display simulations
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         !glfwWindowShouldClose(window)) {

    for(int i = 0; i < numSpring; i++)
    {
		if(sim1)
		{
			applyForces(springs1[i], springs1[i].a, springs1[i].b);
			animateSpring(springs1[i]);
		}
		else if(sim2)
		{
			applyForces(springs2[i], springs2[i].a, springs2[i].b);
			animateSpring(springs2[i]);
		}
		else if(sim3)
		{
			applyForces(springs3[i], springs3[i].a, springs3[i].b);	
			animateSpring(springs3[i]);
		}	
		else if(sim4)
		{
			applyForces(springs4[i], springs4[i].a, springs4[i].b);				
			animateSpring(springs4[i]);
		}
	}
	
	for(int i = 0; i < numMass; i++)
	{
		if(sim1)
		{
			resolveForces(&masses1[i]);
			animateQuad(masses1[i]);
		}
		else if(sim2)
		{
			resolveForces(&masses2[i]);	
			animateQuad(masses2[i]);
		}
		else if(sim3)
		{
			resolveForces(&masses3[i]);	
			animateQuad(masses3[i]);
		}	
		else if(sim4)
		{
			resolveForces(&masses4[i]);
			animateQuad(masses4[i]);
		}
	}
	
    displayFunc();
    verts.clear();	
    verts2.clear();
    	
    moveCamera();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // clean up after loop
  deleteIDs();
  return 0;
}

//==================== CALLBACK FUNCTIONS ====================//

void windowSetSizeFunc(GLFWwindow *window, int width, int height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  reloadProjectionMatrix();
  setupModelViewProjectionTransform();
  reloadMVPUniform();
}

void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height) {
  FB_WIDTH = width;
  FB_HEIGHT = height;

  glViewport(0, 0, FB_WIDTH, FB_HEIGHT);
}

void windowMouseButtonFunc(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      g_cursorLocked = GL_TRUE;
    } else {
      g_cursorLocked = GL_FALSE;
    }
  }
}

void windowMouseMotionFunc(GLFWwindow *window, double x, double y) {
  if (g_cursorLocked) {
    float deltaX = (x - g_cursorX) * 0.01;
    float deltaY = (y - g_cursorY) * 0.01;
    camera.rotateAroundFocus(deltaX, deltaY);

    reloadViewMatrix();
    setupModelViewProjectionTransform();
    reloadMVPUniform();
  }

  g_cursorX = x;
  g_cursorY = y;
}

void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods) {
  bool set = action != GLFW_RELEASE && GLFW_REPEAT;
  switch (key) {
  case GLFW_KEY_ESCAPE:
    glfwSetWindowShouldClose(window, GL_TRUE);
    break;
  case GLFW_KEY_W:
    g_moveBackForward = set ? 1 : 0;
    break;
  case GLFW_KEY_S:
    g_moveBackForward = set ? -1 : 0;
    break;
  case GLFW_KEY_A:
    g_moveLeftRight = set ? 1 : 0;
    break;
  case GLFW_KEY_D:
    g_moveLeftRight = set ? -1 : 0;
    break;
  case GLFW_KEY_Q:
    g_moveUpDown = set ? -1 : 0;
    break;
  case GLFW_KEY_E:
    g_moveUpDown = set ? 1 : 0;
    break;
  case GLFW_KEY_1:
	initSim1();
	break;
  case GLFW_KEY_2:
    initSim2();
    break;
  case GLFW_KEY_3:
	initSim3();
	break;
  case GLFW_KEY_4:
    initSim4();
    break;
  default:
    break;
  }
}

//==================== OPENGL HELPER FUNCTIONS ====================//

void moveCamera() {
  Vec3f dir;

  if (g_moveBackForward) {
    dir += Vec3f(0, 0, g_moveBackForward * g_panningSpeed);
  }
  if (g_moveLeftRight) {
    dir += Vec3f(g_moveLeftRight * g_panningSpeed, 0, 0);
  }
  if (g_moveUpDown) {
    dir += Vec3f(0, g_moveUpDown * g_panningSpeed, 0);
  }

  if (g_rotateUpDown) {
    camera.rotateUpDown(g_rotateUpDown * g_rotationSpeed);
  }
  if (g_rotateLeftRight) {
    camera.rotateLeftRight(g_rotateLeftRight * g_rotationSpeed);
  }
  if (g_rotateRoll) {
    camera.rotateRoll(g_rotateRoll * g_rotationSpeed);
  }

  if (g_moveUpDown || g_moveLeftRight || g_moveBackForward ||
      g_rotateLeftRight || g_rotateUpDown || g_rotateRoll) {
    camera.move(dir);
    reloadViewMatrix();
    setupModelViewProjectionTransform();
    reloadMVPUniform();
  }
}

std::string GL_ERROR() {
  GLenum code = glGetError();

  switch (code) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  default:
    return "Non Valid Error Code";
  }
}
