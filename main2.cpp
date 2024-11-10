

// my include 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "MyShader.h"
#include "MyComputeShader.h"




#include "src/RenderWidgets/RenderingOrderExp.h"

// the include of "glad" must be before the "glfw"
#include <GLFW\glfw3.h>
#include <iostream>

#define IMGUI_ENABLED
#define VSYNC_DISABLED

#ifdef IMGUI_ENABLED
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>
#endif // IMGUI_ENABLED


#pragma comment (lib, "lib-vc2015\\glfw3.lib")
#pragma comment(lib, "assimp-vc141-mt.lib")

const int FRAME_WIDTH = 500;
const int FRAME_HEIGHT = 500;



void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void cursorPosCallback(GLFWwindow* window, double x, double y);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool initializeGL();
void resizeGL(GLFWwindow* window, int w, int h);
void paintGL();
void updateState();

INANOA::RenderingOrderExp* m_renderWidget = nullptr;

double PROGRAM_FPS = 0.0;
double FRAME_MS = 0.0;


glm::vec4 mouseCursor;
void mouse_callback(GLFWwindow* window, double x, double y) {
	mouseCursor.x = x;
	mouseCursor.y = FRAME_HEIGHT-y;

}



// Quad 
const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;
unsigned int quadVAO = 0;
//unsigned int quadVBO;
unsigned int ssboHandle;

void renderQuad()
{
	if (quadVAO == 0)
	{


		GLfloat vertices[] = {
			// x, y, z positions for each vertex
		   -0.5f,  -0.5f, 0.0f, 0.0f,
		   -0.5f,   0.5f, 0.0f, 0.0f,
			0.5f,  -0.5f, 0.0f, 0.0f, 
			0.5,    0.5,  0.0f, 0.0f,

			// Add as many vertices as needed
		};

		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		GLuint ssbo;
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_MAP_READ_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);  // Bind to buffer binding point 0

	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(FRAME_WIDTH, FRAME_HEIGHT, "rendering", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback);

	glfwMakeContextCurrent(window);

	// load OpenGL function pointer
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// my shaders
	MyShader screenQuad("vertexSource.vert", "fragmentSource.frag");
	//MyComputeShader computeShader("computeSource.comp");


	


	while (!glfwWindowShouldClose(window)) {
		
	

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		screenQuad.use();
		renderQuad();


		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}



bool initializeGL() {

	// initialize render widget
	m_renderWidget = new INANOA::RenderingOrderExp();
	if (m_renderWidget->init(FRAME_WIDTH, FRAME_HEIGHT) == false) {
		return false;
	}

	return true;
}
void resizeGL(GLFWwindow* window, int w, int h) {
	m_renderWidget->resize(w, h);
}
void updateState() {
	m_renderWidget->update();
}
void paintGL() {

#ifdef IMGUI_ENABLED
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
#endif // IMGUI_ENABLED

	m_renderWidget->render();

	static char fpsBuf[] = "fps: 000000000.000000000";
	static char msBuf[] = "ms: 000000000.000000000";
	sprintf_s(fpsBuf + 5, 16, "%.5f", PROGRAM_FPS);
	sprintf_s(msBuf + 4, 16, "%.5f", (1000.0 / PROGRAM_FPS));

#ifdef IMGUI_ENABLED	
	ImGui::Begin("Information");
	ImGui::Text(fpsBuf);
	ImGui::Text(msBuf);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI_ENABLED
}

////////////////////////////////////////////////
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {}
}
void cursorPosCallback(GLFWwindow* window, double x, double y) {}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {}

	else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {}
}
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}