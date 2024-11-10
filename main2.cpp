

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
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 1.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 1.0f,   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        2, 3, 0   // second Triangle
    };
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
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(6);
   
    // texture coord attribute
    glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(8);



    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* textureArrayData = stbi_load("church.png", &width, &height, &nrChannels, 0);
    if (textureArrayData) { std::cout << "loading successful" << std::endl; }
    //stbi_image_free(textureArrayData);
    if (width != TEXTURE_WIDTH) {
        std::cout << "width error: " << width << std::endl;
    }
    if (height != TEXTURE_HEIGHT) {
        std::cout << "height error:" << height << std::endl;
    }

    unsigned int textureArrayHandle;
    glGenTextures(1, &textureArrayHandle);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);

    // the internal format for glTexStorageXD must be "Sized Internal Formats¡§
    // max mipmap level = log2(1024) + 1 = 11
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, NUM_TEXTURE);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, TEXTURE_WIDTH, TEXTURE_HEIGHT, 1, GL_RGBA,GL_UNSIGNED_BYTE, textureArrayData);
    //glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, NUM_TEXTURE, GL_RGBA, GL_UNSIGNED_BYTE, textureArrayData);

    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // IMPORTANT !! Use mipmap for the foliage rendering
    //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);


    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

}


void renderQuad(MyShader& screenQuad) {
    
    screenQuad.use();
    glUniform1i(glGetUniformLocation(screenQuad.ID, "ourTexture"), 0);
    glUniform1i(glGetUniformLocation(screenQuad.ID, "ourFace"), 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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