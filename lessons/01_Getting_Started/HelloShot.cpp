// Description: This is a simple example of how to create a shot game using OpenGL. 
// The game is simple, you have to shoot the target by clicking on it.
// The target will move randomly on the screen.
// The game will end when time is up to 60 seconds.

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Shader.h"
#include "Camera.h"

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

const char* aRoomVShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec2 aTexCoord;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

		out vec2 TexCoord;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            TexCoord = vec2(aTexCoord.x, aTexCoord.y);
        }
	)";

const char* aRoomFShaderSource = R"(
		#version 330 core
		out vec4 FragColor;

		in vec2 TexCoord;

		// texture samplers
		uniform sampler2D texture1;
		uniform sampler2D texture2;

		void main()
		{
			// linearly interpolate between both textures (80% container, 20% awesomeface)
			FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.4);
		}
	)";

glm::vec3 aTranVec3 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec4 aTargetOrigin = glm::vec4(0.0f);
glm::vec4 aTargetEdge = glm::vec4(0.0f);

bool MouseLeftButtonPressed = false;

float timeLimit = 10.0;
float beginTime = -1.0f;
int count = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
static GLuint createTexture2D(GLint format, int width, int height, const void* data, GLint filter, GLint wrap);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "HelloShot", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    else {
        // set window locate in the center area of the screen
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowPos(window, (mode->width - 800) / 2, (mode->height - 600) / 2);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);

    // set Shader
    // build and compile our shader zprogram
    // ------------------------------------


    Shader aRoomShader(aRoomVShaderSource, aRoomFShaderSource);

    unsigned int VBO[2], VAO[2];
    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("E:/Lessons/OpenGL/lessons/resources/wall.jpg", &width, &height, &nrChannels, 0);
    unsigned int texture1 = createTexture2D(GL_RGB, width, height, data, GL_LINEAR, GL_REPEAT);
	stbi_image_free(data);

    data = stbi_load("E:/Lessons/OpenGL/lessons/resources/awesomeface.png", &width, &height, &nrChannels, 0);
    unsigned int texture2 = createTexture2D(GL_RGBA, width, height, data, GL_LINEAR, GL_REPEAT);
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    aRoomShader.use();
    aRoomShader.setInt("texture1", 0);
    aRoomShader.setInt("texture2", 1);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            // get matrix's uniform location and set matrix
            aRoomShader.use();

            model = glm::scale(model, glm::vec3(2.0f));
            aRoomShader.setMat4("model", model);

            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.99f));
            aRoomShader.setMat4("view", view);

            projection = glm::perspective(glm::radians(80.0f), (float)800 / 600, 0.1f, 100.0f);
            aRoomShader.setMat4("projection", projection);


            // render container
            glBindVertexArray(VAO[0]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            // get matrix's uniform location and set matrix
            aRoomShader.use();

            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.000001f));
            model = glm::translate(model,  aTranVec3);

            aRoomShader.setMat4("model", model);

            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.99f));
            aRoomShader.setMat4("view", view);

            projection = glm::perspective(glm::radians(80.0f), (float)800 / 600, 0.1f, 100.0f);
            aRoomShader.setMat4("projection", projection);

            aTargetOrigin = projection * view * model * glm::vec4(glm::vec3(0.0f), 1.0f);
            aTargetEdge = projection * view * model * glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
            // render container
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(VAO[0]);
        }
        if (beginTime > 0) {
			float currentTime = glfwGetTime();
			float timeSeconds = currentTime - beginTime;

            // set new window title
            float LeftTime = timeLimit - timeSeconds;
            // set leftTime 0.00 format
            char LeftTimeStr[10];
            sprintf(LeftTimeStr, "%.3f", LeftTime);

            std::cout << "Left Time: " << LeftTimeStr << std::endl;

            std::string title = "HelloShot   Count: " + std::to_string(count) + "  Time: " + LeftTimeStr + "s";
            glfwSetWindowTitle(window, title.c_str());

            if (timeSeconds > timeLimit) {
				std::cout << "Game Over" << std::endl;
 ;
                // set new window title
            std::string title = "HelloShot   Count:  " + std::to_string(count) + "  Game Over.";
                glfwSetWindowTitle(window, title.c_str());

                beginTime = -1.0f;
                count = 0;
			}
		}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
    	MouseLeftButtonPressed = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
        if (MouseLeftButtonPressed) {   
            // if aTargetOrigin is in the range of the mouse click, then the target is hit.
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            
            glm::vec4 aTarget = glm::vec4(xpos / width * 2 - 1, 1 - ypos / height * 2, aTargetOrigin.z, aTargetOrigin.w);

            if (glm::distance(aTarget, aTargetOrigin) < glm::distance(aTargetEdge, aTargetOrigin)) {
				std::cout << "Hit the target" << std::endl;

                float x, y, z;
                x = (rand() % 100) / 100.0f * 2 - 1;
                y = (rand() % 100) / 100.0f * 2 - 1;
                z = (rand() % 100) / 100.0f * 2 - 1;

                aTranVec3 = glm::vec3(x, y, z) * glm::vec3(3.0f);
                count++;
			}
			else {
				std::cout << "Miss the target" << std::endl;
			}
            if (beginTime < 0) { 
                beginTime = glfwGetTime(); 
            }
           
        }
        MouseLeftButtonPressed = false;
	}
}
static GLuint createTexture2D(GLint format, int width, int height, const void* data, GLint filter, GLint wrap)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    return texture;
}