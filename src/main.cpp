#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void process_input(GLFWwindow* window);
void calculate_delta_time();


// Settings
const unsigned int SCREEN_WIDTH = 900;
const unsigned int SCREEN_HEIGHT = 700;
const float FOV_Y = 80.0f;
const float DESIRED_CAMERA_SPEED = 10.0f;
const float SENSITIVITY = 0.1f;

float delta_time = 0.0f; // Time between current frame and last frame
float last_frame_time = 0.0f;

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 camera_front = glm::vec3(0, 0, -1);
glm::vec3 camera_up = glm::vec3(0, 1, 0);

float last_x = SCREEN_WIDTH / 2;
float last_y = SCREEN_HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;
bool first_mouse = true;

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
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Radical Violet Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	// query GPU info
	const char* GPUVendor = (const char*)glGetString(GL_VENDOR);
	const char* GPURenderer = (const char*)glGetString(GL_RENDERER);
	std::cout << "GPU Vendor::" << GPUVendor << std::endl << "GPU Renderer::" << GPURenderer << std::endl;
	// query max GPU vertex attributes
	int nAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttributes);
	std::cout << "GPU Vertex Attributes supported::" << nAttributes << std::endl;
	
	// Shader program
	Shader shaderProgram("shader.vert", "shader.frag");

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
	

	glm::mat4 projection(1.0f);
	projection = glm::perspective(glm::radians(FOV_Y), (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f); // NOTE: aspect ratio will determine FOV_X

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
		// input
		process_input(window);

		// render commands
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		calculate_delta_time();

		glm::mat4 view = glm::mat4(1);
		view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);


		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);

		// Drawing
		glBindVertexArray(VAO);
		shaderProgram.use();
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePosition[i]);
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0));
			shaderProgram.setMat4("model", model);
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
	if(first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}

	float x_offset = xpos - last_x;
	float y_offset = last_y - ypos; // reversed since y-coordinates range from bottom to top
	last_x = xpos;
	last_y = ypos;

	x_offset *= SENSITIVITY;
	y_offset *= SENSITIVITY;

	yaw += x_offset;
	pitch += y_offset;

	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera_front = glm::normalize(direction);
}

// Handles all input within GLFW window
void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Movement
	float camera_speed = DESIRED_CAMERA_SPEED * delta_time;

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera_pos += camera_speed * camera_front;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{	// Use cross product to create a right vector accordingly, creates strafe effect.
		camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera_pos -= camera_speed * camera_front;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
	}
}

void calculate_delta_time()
{
	float current_frame_time = glfwGetTime();
	delta_time = current_frame_time - last_frame_time;
	last_frame_time = current_frame_time;
}