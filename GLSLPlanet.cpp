// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>

// GLew
#include "glew.h"

// Include GLFW
#include "glfw3.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLvbo.h"
#include "utils.h"
#include "shaderHelper.h"
#include "TerrainMesh.h"

#include "TextFile.h"
#include <direct.h>
#include <sstream>

// Making this run in nVidia fixes everything, it's fucked on Intel UHD... suspect there's something wrong with the shaders in that case
#if 1
typedef unsigned long       DWORD;
extern "C" {
	// This is the quickest and easiest way to enable using the nVidia GPU on a Windows laptop with a dedicated nVidia GPU and Optimus tech.
	// enable optimus!
	// https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// AMD have one too!!!
	// https://gpuopen.com/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif

namespace NKeyboard {
	enum EKeyStates {
		eKeyUnset=0,
		eKeyPressed,
		eKeyHeld,
		eKeyReleased
	};
	#define NUM_KEYS 256
	EKeyStates g_keys[NUM_KEYS] = {eKeyUnset};
	EKeyStates g_keysPrev[NUM_KEYS] = {eKeyUnset};
	void UpdateKeyStates(GLFWwindow *window) {
		for( int k=0; k<NUM_KEYS; k++ ) 
		{
			g_keysPrev[k] = g_keys[k];
			g_keys[k] = (glfwGetKey(window, k)==GLFW_PRESS) ? eKeyPressed : eKeyReleased;
		}
	}
	EKeyStates GetKeyState( const uint8_t c )
	{
		switch(g_keys[c]) {
		case eKeyUnset:		return eKeyUnset;
		case eKeyPressed:	return g_keysPrev[c]==eKeyPressed ? eKeyHeld : eKeyPressed;
		case eKeyReleased:	return g_keysPrev[c]==eKeyReleased ? eKeyUnset : eKeyReleased;
		default: assert(false && "this shouldn't happen!"); break;
		}
		return eKeyUnset;
	}
};

static bool s_bShouldClose = false;
void WindowCloseFunc(GLFWwindow*)
{
	s_bShouldClose = true;
}

static int mouseW = 0;
void ScrollFunc(GLFWwindow *pWnd, double x, double y)
{
	mouseW = int(y) * 10;
}

struct TFPSCamera
{
	glm::vec3 pos;
	glm::vec3 rot;
} fpsCamera;

int main()
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	
	GLFWmonitor *PrimaryMonitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(PrimaryMonitor);
	const int screen_width = mode->width >> 1;
	const int screen_height = mode->height >> 1;
	const int screen_x_offset = (mode->width - screen_width) >> 1;
	const int screen_y_offset = (mode->height - screen_height) >> 1;

	GLFWwindow *PrimaryWindow = glfwCreateWindow( screen_width, screen_height, "GLSLPlanet", NULL, NULL);

	// Open a window and create its OpenGL context
	if( !PrimaryWindow )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(PrimaryWindow);
	glfwSetWindowCloseCallback(PrimaryWindow, WindowCloseFunc);

	int major, minor, rev;
	glfwGetVersion(&major, &minor, &rev);

	const char* pVerStr = (char*)glGetString(GL_VERSION);
	const char* pVenStr = (char*)glGetString(GL_VENDOR);
	const char* pRenStr = (char*)glGetString(GL_RENDERER);
	const char* pSLVStr = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	std::stringstream log;
	log << "--------------------------\n" << 
		"OpenGL details: \n -- VERSION: \"" << pVerStr << "\"\n -- VENDOR: \"" << pVenStr << "\"\n" <<
		" -- RENDERER: " << pRenStr << "\"\n -- SHADING LANGUAGE VERSION: " << pSLVStr << std::endl;


	//const GLboolean bInitOk = GLeeInit();
	//assert(bInitOk == GL_TRUE);

	glewExperimental = true;
	GLenum glew_err;
	if ((glew_err = glewInit()) != GLEW_OK) {
		log << "GLEW initialisation failed: " << glewGetErrorString(glew_err) << std::endl;
		assert(false);
	}

	_mkdir("./logs");
	textFileWrite("./logs/opengl.log", log.str().c_str());

	int MaxVertexTextureImageUnits;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits);
	int MaxCombinedTextureImageUnits;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);

	glfwSetWindowTitle( PrimaryWindow, "Sphere" );

	glfwSetWindowPos( PrimaryWindow, screen_x_offset, screen_y_offset );

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode( PrimaryWindow, GLFW_STICKY_KEYS, GL_TRUE );
	glfwSetCursorPos( PrimaryWindow, screen_width/2, screen_height/2);

	// Set color and depth clear value
    glClearDepth(1.0f);
	// Dark blue background
	glClearColor(0.0f, 0.05f, 0.1f, 0.0f);
	//glClearColor(100/255.f, 149/255.f, 237/255.f, 0.0f);
	checkGLError();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	checkGLError();

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	checkGLError();
	//glDisable(GL_CULL_FACE);

	// a little aside, this calculates the radius(R) of a torus
	// the generates 1g for a given rpm.
	// the numbers get very big very quickly :(
	/*double rpm = 1.0;
	double cal = ((PI*rpm)/30.0);
	double g = 9.81;
	double R = g / (cal*cal);*/

	GeoSphere *pSphere = new GeoSphere();
	checkGLError();

	////////////////////////////////////////////////////////////////

	fpsCamera.pos = glm::vec3(0.f, 0.f, -37.5f); // Camera is here
	fpsCamera.rot = glm::vec3(0.f, 0.f, 0.f);    // Orientation

	////////////////////////////////////////////////////////////////

    int		screenWide = screen_width;
	int		screenHigh = screen_height;
	float	screenWidef = float(screen_width);
	float	screenHighf = float(screen_height);
	float	aspect = screenWidef / screenHighf;

	int xprev=0, yprev=0;
	float theta=0.0f, phi=0.0f;
#ifdef _DEBUG
	float cube_theta=0.0f, cube_phi=0.0f;
#endif
	float sample_pt_theta=0.0f, sample_pt_phi=0.0f;

	float zoomDist = -15.0f;
	const float geoSphereRadius = 25.0f;

	bool bUseWireframe = false;

	glfwSetScrollCallback(PrimaryWindow, ScrollFunc);

	do {
		////////////////////////////////////////////////////////////////
		// handle resizing the screen/window
		glfwGetWindowSize(PrimaryWindow, &screenWide, &screenHigh);
		screenWidef = float(screenWide);
		screenHighf = float(screenHigh);
		aspect = screenWidef / screenHighf;

		////////////////////////////////////////////////////////////////
		// update the user input
		{
			zoomDist += float(mouseW) * 0.1f;
			zoomDist = Clamp<float>(zoomDist, -100.0f, 0.0f);
			mouseW = 0;
		}

		{
			double x, y;
			glfwGetCursorPos(PrimaryWindow, &x, &y);
			const int xDiff = int(x - double(xprev));
			const int yDiff = int(y - double(yprev));
			xprev = int(x);
			yprev = int(y);
			static const float phi_limit = 80.0f;
			if(GLFW_PRESS==glfwGetMouseButton(PrimaryWindow, GLFW_MOUSE_BUTTON_LEFT)) {
				theta += float(xDiff) * 0.1f;
				phi += float(yDiff) * 0.1f;
				phi = Clamp<float>(phi, -phi_limit, phi_limit);
				fpsCamera.rot.x = phi;
				fpsCamera.rot.y = theta;
			} else if(GLFW_PRESS==glfwGetMouseButton(PrimaryWindow, GLFW_MOUSE_BUTTON_RIGHT)) {
				// rotate/move the campos
				sample_pt_theta -= float(xDiff) * 0.2f;
				sample_pt_phi += float(yDiff) * 0.2f;
				sample_pt_phi = Clamp<float>(sample_pt_phi, -phi_limit, phi_limit);
			} else if(GLFW_PRESS==glfwGetMouseButton(PrimaryWindow, GLFW_MOUSE_BUTTON_MIDDLE)) {
				// rotate/move the campos
				sample_pt_theta -= float(xDiff) * 0.2f;
				sample_pt_phi += float(yDiff) * 0.2f;
				sample_pt_phi = Clamp<float>(sample_pt_phi, -phi_limit, phi_limit);
			}
		}
		
		if( NKeyboard::GetKeyState(GLFW_KEY_Q) == NKeyboard::eKeyPressed ) {
			bUseWireframe = !bUseWireframe;
		}

		////////////////////////////////////////////////////////////////
		// Compute the MVP matrix from keyboard and mouse input
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, aspect, 0.1f, 1000.f);
		
#if 1
		// Polar Camera matrix
		glm::mat4 ViewMatrix;
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.f, 0.f, (-geoSphereRadius) + zoomDist));
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(phi), glm::vec3(1.f, 0.f, 0.f));
		ViewMatrix = glm::rotate(ViewMatrix, glm::radians(theta), glm::vec3(0.f, 1.f, 0.f));
#else
		// fps camera stuff
		glm::mat4 matty(1.0f);
		//matty = glm::rotate(matty, glm::radians(fpsCamera.rot.z), glm::vec3(0.f, 0.f, 1.f));
		matty = glm::rotate(matty, glm::radians(fpsCamera.rot.y), glm::vec3(0.f, 1.f, 0.f));
		matty = glm::rotate(matty, glm::radians(fpsCamera.rot.x), glm::vec3(1.f, 0.f, 0.f));

		const glm::vec3 dir = glm::vec3(matty * glm::vec4(0.f, 0.f, 1.f, 1.f));
		const glm::vec3 up = glm::vec3(matty * glm::vec4(0.f, 1.f, 0.f, 1.f));
		const glm::vec3 right = glm::cross(dir, up);

		if (NKeyboard::GetKeyState('W') == NKeyboard::eKeyHeld)
			fpsCamera.pos += dir;
		else if (NKeyboard::GetKeyState('A') == NKeyboard::eKeyHeld)
			fpsCamera.pos -= right;
		else if (NKeyboard::GetKeyState('S') == NKeyboard::eKeyHeld)
			fpsCamera.pos -= dir;
		else if (NKeyboard::GetKeyState('D') == NKeyboard::eKeyHeld)
			fpsCamera.pos += right;
		
		const glm::vec3 tar = fpsCamera.pos + dir;
		const glm::mat4 ViewMatrix =	glm::lookAt(
											fpsCamera.pos,	// Camera is here
											tar,			// and looks here : at the same position, plus "direction"
											up				// Head is up (set to 0,-1,0 to look upside-down)
										);
#endif
		glm::mat4 ModelMatrix	= glm::mat4(1.0);
		glm::mat4 MVP			= ProjectionMatrix * ViewMatrix * ModelMatrix;

		////////////////////////////////////////////////////////////////
		// update, and possibly render the terrain for the sphere
		glm::mat4 samplePtMat;
		samplePtMat = glm::rotate(samplePtMat, glm::radians(sample_pt_phi), glm::vec3(0.f, 0.f, 1.f));
		samplePtMat = glm::rotate(samplePtMat, glm::radians(sample_pt_theta), glm::vec3(0.f, 1.f, 0.f));
		const glm::vec4 campos(1.0f, 0.0f, 0.0f, 1.0f);
		pSphere->Update(glm::vec3(campos*samplePtMat));

		////////////////////////////////////////////////////////////////
		// Render the main scene
		glViewport(0, 0, screenWide, screenHigh);
		checkGLError();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGLError();
		
		////////////////////////////////////////////////////////////////
		// render the bloody sphere here
		if(bUseWireframe) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		pSphere->Render(ViewMatrix,ModelMatrix,MVP);
		if(bUseWireframe) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		glUseProgram(0);
		checkGLError();

		// Swap buffers
		glfwSwapBuffers(PrimaryWindow);
		checkGLError();
        glfwPollEvents();

		NKeyboard::UpdateKeyStates(PrimaryWindow);

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( PrimaryWindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS && (false == s_bShouldClose) );

	checkGLError();
	if(pSphere) {
		delete pSphere;
		pSphere = nullptr;
	}
	checkGLError();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}




