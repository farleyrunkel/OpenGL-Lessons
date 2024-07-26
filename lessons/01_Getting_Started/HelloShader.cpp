#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>
#include <iostream>

#include "stb_image.h"

#include "Shader.h"

vec4 aColor;
// 生成随机颜色的函数
void setRandomColor(vec4 v) {
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	v[0] = r;
	v[1] = g;
	v[2] = b;
	v[3] = 1.0f;
}
void setContractColor(vec4 v, vec4 s) {
	v[0] = 1.0f - s[0];
	v[1] = 1.0f - s[1];
	v[2] = 1.0f - s[2];
	v[3] = 1.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool isMouseLeftButtonPressed = false;

float vertices[] = {
	-0.5f, 0.5f, 0.0f,   // 左上角
	0.0f, 0.5f, 0.0f,   // 中上角
	0.5f, 0.5f, 0.0f,   // 右上角
	-0.5f, -0.5f, 0.0f,   // 左下角
	0.0f, -0.5f, 0.0f,   // 中下角
	0.5f, -0.5f, 0.0f,   // 右下角
};

unsigned int indices[] = {
	0, 1, 3,
	1, 3, 4,
	1, 2, 4,
	2, 4, 5
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "HelloWindow", NULL, NULL);
	// set window locate in the center area of the screen
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowPos(window, (mode->width - 800) / 2, (mode->height - 600) / 2);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);

	srand(static_cast<unsigned>(time(nullptr))); // 用当前时间初始化随机种子

	setRandomColor(aColor);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 uColor;"
		"void main()\n"
		"{\n"
		"       FragColor = uColor;  \n"
		"}\n\0";

	Shader aShader(vertexShaderSource, fragmentShaderSource);
	aShader.use();
	// 获取 uniform 位置
	GLuint colorLoc = glGetUniformLocation(aShader.ID, "uColor");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	while (!glfwWindowShouldClose(window))
	{
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				std::cout << "Mouse left button pressed" << std::endl;
				isMouseLeftButtonPressed = true;
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
				if (isMouseLeftButtonPressed) {
					isMouseLeftButtonPressed = false;
					setRandomColor(aColor);
				}
			}
		}

		// 更新 uniform 颜色
		glUniform4fv(colorLoc, 1, aColor);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		
		vec4 aContrastColor;
		setContractColor(aContrastColor, aColor);
		// 更新 uniform 颜色
		glUniform4fv(colorLoc, 1, aContrastColor);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
} 