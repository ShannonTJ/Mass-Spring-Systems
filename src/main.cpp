/**
 * OpenGL Template By: 	Andrew Robert Owens
 * Modifications By: 	Shannon TJ
 * Date:				March 23, 2017
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

#define PI 3.14159265359

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

bool sim3;

vector<Vec3f> verts;
vector<Vec3f> verts2;

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
string GL_ERROR();
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

vector<Mass> masses;
vector<Spring> springs;

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
	masses.clear();
	springs.clear();
	
	m = initMass(m, 1.f, true, vec3(0,3.5f,0));
	masses.push_back(m);
	masses[0] = m;
	
	m = initMass(m, 1.f, false, vec3(2.f,2.f,0));	
	masses.push_back(m);
	masses[1] = m;
	
	s = initSpring(s, &masses[0], &masses[1], 25.f, 1.f);	
	springs.push_back(s);
	springs[0] = s;

	numMass = masses.size();
	numSpring = springs.size();
	sim3 = false;
}

//Chain pendulum
void initSim2()
{
	masses.clear();
	springs.clear();
	
	mChain = initMass(mChain, 1.f, true, vec3(0,3.5f,0));	
	masses.push_back(mChain);
	masses[0] = mChain;
	
	mChain = initMass(mChain, 0.5f, false, vec3(0,3.f,0));		
	masses.push_back(mChain);
	masses[1] = mChain;
	
	mChain = initMass(mChain, 0.5f, false, vec3(0.5f,3.5f,0));	
	masses.push_back(mChain);		
	masses[2] = mChain;
	
	mChain = initMass(mChain, 1.5f, false, vec3(1.f,3.5f,0));	
	masses.push_back(mChain);		
	masses[3] = mChain;

	sChain = initSpring(sChain, &masses[0], &masses[1], 25.f, 1.f);	
	springs.push_back(sChain);	
	springs[0] = sChain;
	
	sChain = initSpring(sChain, &masses[1], &masses[2], 25.f, 1.f);	
	springs.push_back(sChain);		
    springs[1] = sChain;
	
	sChain = initSpring(sChain, &masses[2], &masses[3], 25.f, 1.f);	
	springs.push_back(sChain);		
	springs[2] = sChain;
	
	numMass = masses.size();
	numSpring = springs.size();
	sim3 = false;
}

//Jello cube
void initSim3()
{
	masses.clear();
	springs.clear();
	
	int count = 1;
	int count2 = 1;
	
	//Size of cube
	int numCube = 5;
	numMass = numCube*numCube*numCube;
	
	//Mass coordinates
	float originalX = -2.f;
	float x = originalX;
	float originalY = 5.5f;
	float y = originalY;
	float z = 0;
	
	//Size of gap between masses
	float space = 1.f;
	
	//Spring stiffness + restlength
	float k = 1000;
	float rLen = 0;
		
	//Initialize the masses
	for(int i = 0; i < numMass; i++)
	{		
		mCube = initMass(mCube, 1.f, false, vec3(x,y,z));
		masses.push_back(mCube);
		masses[i] = mCube;
		x += space;
		
		//Initialize mass positions
		if(i == (count*numCube - 1))
		{
			if(i/(numCube*numCube*count2 - 1) == 1)
			{
				//Move to a new row (Z axis)
				x = originalX;
				y = originalY;
				z = z - space;
				
				count2++;
				count++;					
			}
			//Move to a new row (Y axis)
			else
			{
				x = originalX;
				y = y - space;
				
				count++;
			}
		}
	}	
	
	//Initialize the springs
	int m = 0;
	for(int i = 0; i < numMass; i++)
	{
		for(int j = 0; j < numMass; j++)
		{
			rLen = getLength(&masses[i], &masses[j]);
			
			//X axis
			if(masses[i].position.x - masses[j].position.x == space && masses[i].position.y == masses[j].position.y)
			{
				if(masses[i].position.z == masses[j].position.z)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;
				}
			}
						
			//Y axis
			if(masses[i].position.y - masses[j].position.y == space && masses[i].position.z == masses[j].position.z)
			{
				if(masses[i].position.x == masses[j].position.x)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;
				}
			}	
			
			//Z axis
			if(masses[i].position.z - masses[j].position.z == space && masses[i].position.y == masses[j].position.y)
			{
				if(masses[i].position.x == masses[j].position.x)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;	
				}
			}
			
			//Side crosses
			if(masses[i].position.z - masses[j].position.z == space && masses[i].position.y - masses[j].position.y == space)
			{
				if(masses[i].position.x == masses[j].position.x)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;		
				}
			}			
			
			if(masses[i].position.z - masses[j].position.z == space && masses[i].position.y - masses[j].position.y == -space)
			{
				if(masses[i].position.x == masses[j].position.x)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;	
				}
			}		
		
			//Horizontal crosses
			if(masses[i].position.z - masses[j].position.z == space && masses[i].position.x - masses[j].position.x == space)
			{
				if(masses[i].position.y == masses[j].position.y)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;	
				}
			}			
			
			if(masses[i].position.z - masses[j].position.z == space && masses[i].position.x - masses[j].position.x == -space)
			{
				if(masses[i].position.y == masses[j].position.y)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;	
				}
			}			
			
			//Front and back crosses
			if(masses[i].position.x - masses[j].position.x == space && masses[i].position.y - masses[j].position.y == space)
			{
				if(masses[i].position.z == masses[j].position.z)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;	
				}
			}			
			
			if(masses[i].position.x - masses[j].position.x == space && masses[i].position.y - masses[j].position.y == -space)
			{
				if(masses[i].position.z == masses[j].position.z)
				{
					sCube = initSpring(sCube, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCube);	
					springs[m] = sCube;
					m++;		
				}
			} 				
		}	
	}
	
	numSpring = springs.size();
	sim3 = true;
}

//Hanging cloth
void initSim4()
{
	masses.clear();
	springs.clear(); 
	
	int count = 1;
	
	//Size of cloth
	int numCloth = 11;
	int numRows = 7;
	numMass = numCloth*numRows;
	
	//Mass coordinates
	float originalX = -2.5f;
	float x = originalX;
	float y = 3.5f;
	float z = 0;
	
	//Size of gap between masses
	float xSpace = 0.5f;
	float zSpace = 1.0f;
	
	//Spring stiffness + rest length
	float k = 200;
	float rLen = 0;
	
	//Initialize the masses
	for(int i = 0; i < numMass; i++)
	{	
		//Initialize fixed points
		if(i % 2 == 0 && i < numCloth)
			mCloth = initMass(mCloth, 1.f, true, vec3(x,y,z));
		//Initialize loose points
		else
			mCloth = initMass(mCloth, 1.f, false, vec3(x,y,z));
		
		masses.push_back(mCloth);
		masses[i] = mCloth;	
		x += xSpace;
		
		//Move to a new row (Z axis)
		if(i == (count*numCloth - 1))
		{
			x = originalX;
			z = z - zSpace;
			count++;
		}
}
	
	//Initialize the springs
	int m = 0;
	for(int i = 0; i < numMass; i++)
	{
		for(int j = 0; j < numMass; j++)
		{
			rLen = getLength(&masses[i], &masses[j]);
			
			//Vertical lines
			if(masses[i].position.x == masses[j].position.x && masses[i].position.z - masses[j].position.z == zSpace)
			{
					sCloth = initSpring(sCloth, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCloth);	
					springs[m] = sCloth;
					m++;
			}
			
			//Horizontal lines
			if(masses[i].position.z == masses[j].position.z && masses[i].position.x - masses[j].position.x == xSpace)
			{
					sCloth = initSpring(sCloth, &masses[i], &masses[j], k, rLen);									
					springs.push_back(sCloth);	
					springs[m] = sCloth;
					m++;
			}
			
			//Crossed lines
			if(masses[i].position.z - masses[j].position.z == zSpace && masses[i].position.x - masses[j].position.x == xSpace)
			{
					sCloth = initSpring(sCloth, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCloth);	
					springs[m] = sCloth;
					m++;
			}			
			
			if(masses[i].position.z - masses[j].position.z == zSpace && masses[i].position.x - masses[j].position.x == -xSpace)
			{
					sCloth = initSpring(sCloth, &masses[i], &masses[j], k, rLen);
					springs.push_back(sCloth);	
					springs[m] = sCloth;
					m++;
			}		
		}
	}
	
	numSpring = springs.size();
	sim3 = false;
}

void applyForces(Spring s, Mass *a, Mass *b)
{
	//Get current length of spring
	float springLength = getLength(s.a, s.b);
	vec3 unitAB = (s.b->position - s.a->position)/springLength;
	
	//hooke = -k(x-x0)AB
	vec3 hooke = (-s.stiffness)*(springLength - s.restLength)*unitAB;
	
	vec3 bAcc = hooke/(s.b->mass);
	vec3 aAcc = -bAcc;

	//Update acceleration for mass *a and *b
	s.b->acc += bAcc;
	s.a->acc += aAcc;
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
			m->velocity.y = 0.f;
			m->position = m->position + m->velocity*timestep;
			m->position.y = -2.f;
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
			applyForces(springs[i], springs[i].a, springs[i].b);
			animateSpring(springs[i]);
	}
	
	for(int i = 0; i < numMass; i++)
	{
			resolveForces(&masses[i]);
			animateQuad(masses[i]);
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
