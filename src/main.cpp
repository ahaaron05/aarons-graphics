#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "Camera.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <stack>

#ifndef M_PI 	// manually defined pi constant for use in calculations
#define M_PI 3.14159265358979323846
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
void calculate_delta_time();
void draw_cube(Shader& shader);
void draw_sierpinski(Shader& shader, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int degree);
void drawTexturedTriangle(Shader& shader, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);


// Settings
const unsigned int SCREEN_WIDTH = 900;
const unsigned int SCREEN_HEIGHT = 700;

// cameras
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = SCREEN_WIDTH / 2.0f;
float last_y = SCREEN_HEIGHT / 2.0f;
bool first_mouse = true;

// timing
float delta_time = 0.0f; // Time between current frame and last frame
float last_frame_time = 0.0f;

// lighting
glm::vec3 lightPos(1.3f, 1.2f, -1.0f);

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

	// imgui setup stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// texture stuff
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nChannels;
	unsigned char* data = stbi_load("D:/aarons graphics/res/wall.jpg", &width, &height, &nChannels, 0);
	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	

	// setup for color cube 
	float vertices[] = 
	{	 // Pos					// normals
    	-0.5f, -0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,
    	 0.5f, -0.5f, -0.5f,  	0.0f,  0.0f, -1.0f, 
    	 0.5f,  0.5f, -0.5f,  	0.0f,  0.0f, -1.0f, 			// face 1
    	 0.5f,  0.5f, -0.5f,  	0.0f,  0.0f, -1.0f, 
    	-0.5f,  0.5f, -0.5f,  	0.0f,  0.0f, -1.0f, 
    	-0.5f, -0.5f, -0.5f,  	0.0f,  0.0f, -1.0f, 

    	-0.5f, -0.5f,  0.5f,  	0.0f,  0.0f, 1.0f,
    	 0.5f, -0.5f,  0.5f,  	0.0f,  0.0f, 1.0f,
    	 0.5f,  0.5f,  0.5f,  	0.0f,  0.0f, 1.0f,				// face 2
    	 0.5f,  0.5f,  0.5f,  	0.0f,  0.0f, 1.0f,
    	-0.5f,  0.5f,  0.5f,  	0.0f,  0.0f, 1.0f,
    	-0.5f, -0.5f,  0.5f,  	0.0f,  0.0f, 1.0f,

    	-0.5f,  0.5f,  0.5f, 	-1.0f,  0.0f,  0.0f,
    	-0.5f,  0.5f, -0.5f, 	-1.0f,  0.0f,  0.0f,
    	-0.5f, -0.5f, -0.5f, 	-1.0f,  0.0f,  0.0f,			// face 3
    	-0.5f, -0.5f, -0.5f, 	-1.0f,  0.0f,  0.0f,
    	-0.5f, -0.5f,  0.5f, 	-1.0f,  0.0f,  0.0f,
    	-0.5f,  0.5f,  0.5f, 	-1.0f,  0.0f,  0.0f,

    	 0.5f,  0.5f,  0.5f,  	1.0f,  0.0f,  0.0f,
    	 0.5f,  0.5f, -0.5f,  	1.0f,  0.0f,  0.0f,
    	 0.5f, -0.5f, -0.5f,  	1.0f,  0.0f,  0.0f,				// face 4
    	 0.5f, -0.5f, -0.5f,  	1.0f,  0.0f,  0.0f,
    	 0.5f, -0.5f,  0.5f,  	1.0f,  0.0f,  0.0f,
    	 0.5f,  0.5f,  0.5f,  	1.0f,  0.0f,  0.0f,

    	-0.5f, -0.5f, -0.5f,  	0.0f, -1.0f,  0.0f,
    	 0.5f, -0.5f, -0.5f,  	0.0f, -1.0f,  0.0f,
    	 0.5f, -0.5f,  0.5f,  	0.0f, -1.0f,  0.0f,
    	 0.5f, -0.5f,  0.5f,  	0.0f, -1.0f,  0.0f,				// face 5
    	-0.5f, -0.5f,  0.5f,  	0.0f, -1.0f,  0.0f,
    	-0.5f, -0.5f, -0.5f,  	0.0f, -1.0f,  0.0f,

    	-0.5f,  0.5f, -0.5f,  	0.0f,  1.0f,  0.0f,
    	 0.5f,  0.5f, -0.5f,  	0.0f,  1.0f,  0.0f,
    	 0.5f,  0.5f,  0.5f,  	0.0f,  1.0f,  0.0f,				// face 6
    	 0.5f,  0.5f,  0.5f,  	0.0f,  1.0f,  0.0f,
    	-0.5f,  0.5f,  0.5f,  	0.0f,  1.0f,  0.0f,
    	-0.5f,  0.5f, -0.5f,  	0.0f,  1.0f,  0.0f
	};
	unsigned int colorCubeVAO, VBO;
	glGenVertexArrays(1, &colorCubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(colorCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// pos attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// setup for rendering normal lines
	std::vector<float> normalLinesVerticies;
	float normalLineLength = 0.5f; // for visualization, not for real interpretation;
	for(size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += 6)
	{
		float vx = vertices[i];
		float vy = vertices[i+1];
		float vz = vertices[i+2];

		float nx = vertices[i+3];
		float ny = vertices[i+4];
		float nz = vertices[i+5];

		// start point (vertex position)
		normalLinesVerticies.push_back(vx);
		normalLinesVerticies.push_back(vy);
		normalLinesVerticies.push_back(vz);

		// End point (vertex position + normal * scale)
		normalLinesVerticies.push_back(vx + nx * normalLineLength);
		normalLinesVerticies.push_back(vy + ny * normalLineLength);
		normalLinesVerticies.push_back(vz + nz * normalLineLength);
	}

	unsigned int normalLinesVAO, normalLinesVBO;
	glGenVertexArrays(1, &normalLinesVAO);
	glGenBuffers(1, &normalLinesVBO);
	glBindVertexArray(normalLinesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, normalLinesVBO);
	glBufferData(GL_ARRAY_BUFFER, normalLinesVerticies.size() * sizeof(float), normalLinesVerticies.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Setup for light source cube
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// pos attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader colorObjShader("shaders/color_cube.vert", "shaders/color_cube.frag");
	Shader lightSrcShader("shaders/light_cube.vert", "shaders/light_cube.frag");
	Shader normalLinesShader("shaders/normal_lines.vert", "shaders/normal_lines.frag");
	
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

		colorObjShader.use();
        colorObjShader.setVec3("viewPos", camera.position);
		
		// light properties
		float radius = 4.0f;
		glm::vec3 dynamicLightPos = glm::vec3(radius * cos(static_cast<float>(glfwGetTime()) / 2), 0.0f, radius * sin(static_cast<float>(glfwGetTime()) / 2));
		colorObjShader.setVec3("light.position", dynamicLightPos);
        colorObjShader.setVec3("light.ambient", glm::vec3(0.1f));
        colorObjShader.setVec3("light.diffuse", glm::vec3(0.5f));
        colorObjShader.setVec3("light.specular", glm::vec3(1.0f));

        // material properties (example, chrome)
        colorObjShader.setVec3("material.ambient", glm::vec3(0.25f));
        colorObjShader.setVec3("material.diffuse", glm::vec3(0.4f));
        colorObjShader.setVec3("material.specular", glm::vec3(0.774597f)); // specular lighting doesn't have full effect on this object's material
        colorObjShader.setFloat("material.shininess", 0.6f * 128.0f);
	
		// pass projection matrix to shader (note: in this case, it can change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f); // NOTE: aspect ratio will determine FOV_X
		glm::mat4 view = camera.get_view_matrix();
		colorObjShader.setMat4("projection", projection);
		colorObjShader.setMat4("view", view);  

		glm::mat4 model = glm::mat4(1.0f);
		colorObjShader.setMat4("model", model);
		glm::mat4 normalMat = glm::transpose(glm::inverse(model));
		colorObjShader.setMat4("normalMat", normalMat);
		colorObjShader.setVec3("viewPos", camera.position);


		// render the cube
		glBindVertexArray(colorCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// now render the light source cube
		lightSrcShader.use();
		lightSrcShader.setMat4("projection", projection);
		lightSrcShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos); 	// move and scale down light source
		model = glm::scale(model, glm::vec3(0.2f));
		lightSrcShader.setMat4("model", model);
		model = glm::mat4(1.0f);
		model = glm::translate(model, dynamicLightPos);
		lightSrcShader.setMat4("model", model);
		
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render normal lines visually
		normalLinesShader.use();
		normalLinesShader.setMat4("projection", projection);
		normalLinesShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		normalLinesShader.setMat4("model", model);

		glBindVertexArray(normalLinesVAO);
		glDrawArrays(GL_LINES, 0, normalLinesVerticies.size() / 3);


		// check for events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 

	// de-allocate and clean-up
	glDeleteVertexArrays(1, &colorCubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
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

void draw_cube(Shader& shader)
{
	float vertices[] =
	{   // positions			  //colors
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

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);
	shader.use();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

// Draws a sierpinski triangle to specified degree of depth
void draw_sierpinski(Shader& shader, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int degree)
{
	std::vector<float> vertices;
	
	// lamnda func to add tri to vertex list
	auto add_triangle = [&vertices](glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
	{
		vertices.push_back(v1.x); vertices.push_back(v1.y); vertices.push_back(v1.z);	vertices.push_back(1); vertices.push_back(0); vertices.push_back(0);  // RED
		vertices.push_back(v2.x); vertices.push_back(v2.y); vertices.push_back(v2.z);	vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);  // GREEN
		vertices.push_back(v3.x); vertices.push_back(v3.y); vertices.push_back(v3.z);	vertices.push_back(0); vertices.push_back(0); vertices.push_back(1);  // BLUE
	};

	// use a stack to handle tri's iteratively
	std::stack<std::tuple<glm::vec3, glm::vec3, glm::vec3, int>> stack;
	stack.push({v1, v2, v3, degree});

	while(!stack.empty())
	{
		auto [v1, v2, v3, depth] = stack.top();
		stack.pop();

		if(depth == 0)
		{
			add_triangle(v1, v2, v3);
		}
		else 
		{
			glm::vec3 mid1 = 0.5f * (v1 + v2);
			glm::vec3 mid2 = 0.5f * (v2 + v3);
			glm::vec3 mid3 = 0.5f * (v1 + v3);

			// push the 3 smaller tris to the stack
			stack.push({v1, mid1, mid3, depth - 1});
			stack.push({mid1, v2, mid2, depth - 1});
			stack.push({mid3, mid2, v3, depth - 1});
		}
	}

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	shader.use();
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);

	// cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void drawTexturedTriangle(Shader& shader, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	float vertices[] =
	{	// pos				// color			// tex
		v1.x, v1.y, v1.z,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		v2.x, v2.y, v2.z,	1.0f, 0.0f, 0.0f,	0.5f, 1.0f,
		v3.x, v3.y, v3.z, 	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO);
	shader.use();
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}