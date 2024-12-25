#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "Camera.hpp"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
void calculate_delta_time();


// Settings
const unsigned int SCREEN_WIDTH = 900;
const unsigned int SCREEN_HEIGHT = 700;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = SCREEN_WIDTH / 2.0f;
float last_y = SCREEN_HEIGHT / 2.0f;
bool first_mouse = true;

// timing
float delta_time = 0.0f; // Time between current frame and last frame
float last_frame_time = 0.0f;

int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "aarons graphics", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	// query GPU info
	const char* gpu_vendor = (const char*)glGetString(GL_VENDOR);
	const char* gpu_renderer = (const char*)glGetString(GL_RENDERER);
	std::cout << "GPU Vendor::" << gpu_vendor << std::endl << "GPU Renderer::" << gpu_renderer << std::endl;
	// query max GPU vertex attributes
	int nAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttributes);
	std::cout << "GPU Vertex Attributes supported::" << nAttributes << std::endl;
	
	// Shader program
	Shader shader_program("shader.vert", "shader.frag");

	// Vertex data
	float vertices[] =
	{   // positions				colors
		-0.5f, -0.5f, -0.5f,	  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	  0.0f, 1.0f, 0.0f,
								  
		-0.5f, -0.5f,  0.5f,	  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	  0.0f, 1.0f, 0.0f,
								  
		-0.5f,  0.5f,  0.5f,	  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	  0.0f, 1.0f, 0.0f,
								  
		 0.5f,  0.5f,  0.5f,	  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f, 1.0f, 0.0f,
								  
		-0.5f, -0.5f, -0.5f,	  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	  0.0f, 1.0f, 0.0f,
								  
		-0.5f,  0.5f, -0.5f,	  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	  0.0f, 1.0f, 0.0f,
	};
	unsigned int indices[] =
	{
		0, 1, 3,	// tri 1
		1, 2, 3,	// tri 2
	};
	unsigned int VBO, VAO;
	unsigned int EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO); //! BIND VAO FIRST
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0); // unbind for safety
	
	glm::vec3 cubePosition[]
	{
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f),
	};

	
	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		calculate_delta_time();

		// input
		process_input(window);

		// render commands
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		// pass projection matrix to shader (note: in this case, it can change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f); // NOTE: aspect ratio will determine FOV_X
		shader_program.setMat4("projection", projection);

		// camera/view transformation and pass to shader
		glm::mat4 view = camera.get_view_matrix();
		shader_program.setMat4("view", view);

		// Drawing
		glBindVertexArray(VAO);
		shader_program.use();
		for(int i = 0; i < 10; i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePosition[i]);
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
			shader_program.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		} 

		// check for events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 

	// deallocate
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

// Allows for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// mouse input related stuff
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float x_pos = static_cast<float>(xpos);
    float y_pos = static_cast<float>(ypos);

    if (first_mouse)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

    last_x = x_pos;
    last_y = y_pos;

    camera.process_mouse_input(x_offset, y_offset);
}

// Handles all input within GLFW window
void process_input(GLFWwindow* window)
{
	// Exit program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Line mode Toggle
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Movement
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.process_keyboard_input(FORWARD, delta_time);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.process_keyboard_input(LEFT, delta_time);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.process_keyboard_input(BACKWARD, delta_time);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.process_keyboard_input(RIGHT, delta_time);
}

void calculate_delta_time()
{
	float current_frame_time = glfwGetTime();
	delta_time = current_frame_time - last_frame_time;
	last_frame_time = current_frame_time;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
	camera.process_mouse_scroll(static_cast<float>(y_offset));
}