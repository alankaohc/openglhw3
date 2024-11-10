

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

const int FRAME_WIDTH = 600;
const int FRAME_HEIGHT = 600;




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
const unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;
const int NUM_TEXTURE = 3;

unsigned int quadVAO = 0;
//unsigned int quadVBO;
unsigned int ssboHandle;

void initQuad()
{
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          
         0.8f,  0.8f, 0.0f,     // top right
         0.8f, -0.2f, 0.0f,     // bottom right
        -0.2f, -0.2f, 0.0f,     // bottom left
        -0.2f,  0.8f, 0.0f,     // top left 

		-0.2f,  -0.2f, 0.0f,     // top right
		 -0.2f, -0.8f, 0.0f,      // bottom right
		-0.8f, -0.8f, 0.0f,       // bottom left
		-0.8f,  -0.2f, 0.0f,      // top left 
    };
    unsigned int indices[] = {  
        0, 1, 2,
        2, 3, 0,

		0, 1, 2,
		2, 3, 0,

    };

	struct DrawElementsIndirectCommand {
		// number of index need to draw
		GLuint count;
		// number of instance
		GLuint instanceCount;
		// location of the first index
		GLuint firstIndex;
		// a constant that is added to each index
		GLuint baseVertex;
		// a base instance for fetching instanced vertex attributes
		GLuint baseInstance;
	};

	// prepare draw commands
	DrawElementsIndirectCommand drawCommands[2];
	drawCommands[0].count = 6;
	drawCommands[0].instanceCount = 1;
	drawCommands[0].firstIndex = 0;
	drawCommands[0].baseVertex = 0;
	drawCommands[0].baseInstance = 0;
	drawCommands[1].count = 6;
	drawCommands[1].instanceCount = 1;
	drawCommands[1].firstIndex = 6; // offset = firstIndex * sizeof(type)
	drawCommands[1].baseVertex = 4;
	drawCommands[1].baseInstance = 0;

	// initialize indirect buffer
	/*unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommands), drawCommands, GL_DYNAMIC_DRAW);*/
	
	unsigned int SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(drawCommands), drawCommands,GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);



    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(6);
   



    glBindVertexArray(VAO); 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, SSBO);


}


void renderQuad(MyShader& screenQuad) {
    
    screenQuad.use();
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, 2, 0);
	//int counts[] = { 6, 6 };
	//GLvoid* indices[] = { (GLvoid*)0, (GLvoid*)24 };
	//int baseVertices[] = { 0, 4 };
	//glMultiDrawElements(GL_TRIANGLES, counts, GL_UNSIGNED_INT, indices, 2);
	//glMultiDrawElementsBaseVertex(GL_TRIANGLES, counts, GL_UNSIGNED_INT, indices, 2, baseVertices);
    //glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

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


    initQuad();


	while (!glfwWindowShouldClose(window)) {
		
	
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        renderQuad(screenQuad);

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