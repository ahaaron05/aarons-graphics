#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "Camera.hpp"
#include "stb_image.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>

#define UI_ENABLED 0
#define RENDER_NORMALS 0

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
unsigned int loadTexture(char const* path);


// Settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// cameras
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = SCREEN_WIDTH / 2.0f;
float last_y = SCREEN_HEIGHT / 2.0f;
bool first_mouse = true;

// timing
float delta_time = 0.0f; // Time between current frame and last frame
float last_frame_time = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 1.8f, 3.0f);

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
	#if UI_ENABLED
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true); // setup Platform/Renderer backends
		ImGui_ImplOpenGL3_Init("#version 330");
	#endif

	// setup for color cube 
	float vertices[] = 
	{	// positions          	// normals           	// texture coords
        -0.5f, -0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,  	0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,  	1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,  	1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,  	1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  	0.0f,  0.0f, -1.0f,  	0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  	0.0f,  0.0f,  1.0f,  	0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  	0.0f,  0.0f,  1.0f,  	1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  	0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  	0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  	0.0f,  0.0f,  1.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  	0.0f,  0.0f,  1.0f,  	0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -	1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -	1.0f,  0.0f,  0.0f,  	1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -	1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -	1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -	1.0f,  0.0f,  0.0f,  	0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -	1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  	1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  	1.0f,  0.0f,  0.0f,  	1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  	1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  	1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  	1.0f,  0.0f,  0.0f,  	0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  	1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  	0.0f, -1.0f,  0.0f,  	0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  	0.0f, -1.0f,  0.0f,  	1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  	0.0f, -1.0f,  0.0f,  	1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  	0.0f, -1.0f,  0.0f,  	1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  	0.0f, -1.0f,  0.0f,  	0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  	0.0f, -1.0f,  0.0f,  	0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  	0.0f,  1.0f,  0.0f,  	0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  	0.0f,  1.0f,  0.0f,  	1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  	0.0f,  1.0f,  0.0f,  	1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  	0.0f,  1.0f,  0.0f,  	1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  	0.0f,  1.0f,  0.0f,  	0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  	0.0f,  1.0f,  0.0f,  	0.0f,  1.0f,
	};	
	unsigned int colorCubeVAO, VBO;
	glGenVertexArrays(1, &colorCubeVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(colorCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// pos attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// diffuse map texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// setup for rendering normal lines
	std::vector<float> normalLinesVerticies;
	float normalLineLength = 0.2f; // for visualization, not for real interpretation;
	for(size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += 8)
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Floor setup
	float floor_verticies[] =
	{	// pos	
		-2.0f, 0.0f, 2.0f,		
		-2.0f, 0.0f, -2.0f,		// tri 1
		2.0f, 0.0f, -2.0f,
		
		-2.0f, 0.0f, 2.0f,
		2.0f, 0.0f, 2.0f,		// tri 2
		2.0f, 0.0f, -2.0f,
	};
	unsigned int floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_verticies), floor_verticies, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// setup shaders
	Shader colorObjShader("shaders/color_cube.vert", "shaders/color_cube.frag");
	Shader lightSrcShader("shaders/light_cube.vert", "shaders/light_cube.frag");
	Shader normalLinesShader("shaders/normal_lines.vert", "shaders/normal_lines.frag");
	Shader floorShader("shaders/floor.vert", "shaders/floor.frag");
	
	// load textures
	unsigned int diffuseMap = loadTexture("D:/aarons graphics/res/container2.png");
	unsigned int specularMap = loadTexture("D:/aarons graphics/res/container2_specular.png");
	//unsigned int emissionMap = loadTexture("D:/aarons graphics/res/matrix_emission_map.jpg");

	// pass in uniforms
	colorObjShader.use();
	colorObjShader.setInt("material.diffuseMap", 0);
	colorObjShader.setInt("material.specularMap", 1);
	//colorObjShader.setInt("material.emissionMap", 2);

	glm::vec3 cubePositions[] = 
	{
		glm::vec3( 0.0f,  0.0f,  0.0f),
    	glm::vec3( 2.0f,  5.0f, -15.0f),
    	glm::vec3(-1.5f, -2.2f, -2.5f),
    	glm::vec3(-3.8f, -2.0f, -12.3f),
    	glm::vec3( 2.4f, -0.4f, -3.5f),
    	glm::vec3(-1.7f,  3.0f, -7.5f),
    	glm::vec3( 1.3f, -2.0f, -2.5f),
    	glm::vec3( 1.5f,  2.0f, -2.5f),
    	glm::vec3( 1.5f,  0.2f, -1.5f),
    	glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	
	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		calculate_delta_time();

		#if UI_ENABLED
			// Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow(); // :)
		#endif

		// input
		process_input(window);

		// render commands
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		colorObjShader.use();
        colorObjShader.setVec3("viewPos", camera.position);
        colorObjShader.setFloat("material.shininess", 0.6f * 128.0f);
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		// bind emission map
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, emissionMap);
		
		// light properties
		float radius = 4.0f;
		glm::vec3 dynamicLightPos = glm::vec3(radius * cos(static_cast<float>(glfwGetTime()) / 2), 0.0f, radius * sin(static_cast<float>(glfwGetTime()) / 2));
		colorObjShader.setVec3("light.position", camera.position);
		colorObjShader.setVec3("light.direction", camera.front);
		colorObjShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

        colorObjShader.setVec3("light.ambient", glm::vec3(0.1f));
        colorObjShader.setVec3("light.diffuse", glm::vec3(0.5f));
        colorObjShader.setVec3("light.specular", glm::vec3(1.0f));
		colorObjShader.setFloat("light.constant", 1.0f);
		colorObjShader.setFloat("light.linear", 0.09f);
		colorObjShader.setFloat("light.quadratic", 0.032f);

		// pass projection matrix to shader (note: in this case, it can change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f); // NOTE: aspect ratio will determine FOV_X
		glm::mat4 view = camera.get_view_matrix();
		colorObjShader.setMat4("projection", projection);
		colorObjShader.setMat4("view", view);  


		glm::mat4 model(1.0f); // quick reset

		// render floor
		floorShader.use();
		floorShader.setMat4("projection", projection);
		floorShader.setMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		floorShader.setMat4("model", model);

		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		

		// render the 10 cubes
		for(int i = 0; i < 10; i++)
		{
			colorObjShader.use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			colorObjShader.setMat4("model", model);
			glm::mat4 normalMat = glm::transpose(glm::inverse(model));
			colorObjShader.setMat4("normalMat", normalMat);

			glBindVertexArray(colorCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);


			// render normal lines visually
			#if RENDER_NORMALS 
			normalLinesShader.use();
			normalLinesShader.setMat4("projection", projection);
			normalLinesShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			normalLinesShader.setMat4("model", model);

			glBindVertexArray(normalLinesVAO);
			glDrawArrays(GL_LINES, 0, normalLinesVerticies.size() / 3);
			#endif
		}

		colorObjShader.setVec3("viewPos", camera.position);
		
		// now render the light source cube
		// lightSrcShader.use();
		// lightSrcShader.setMat4("projection", projection);
		// lightSrcShader.setMat4("view", view);
		// model = glm::mat4(1.0f);
		// model = glm::translate(model, lightPos);
		// model = glm::scale(model, glm::vec3(0.2f));
		// lightSrcShader.setMat4("model", model);
		
		// glBindVertexArray(lightCubeVAO);
		// glDrawArrays(GL_TRIANGLES, 0, 36);


		// ImGui render
		#if UI_ENABLED
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		#endif

		// check for events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 

	// de-allocate and clean-up
	glDeleteVertexArrays(1, &colorCubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);
	#if UI_ENABLED
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	#endif
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

// loads and formats a 2D texture from file
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	
	int width, height, nComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nComponents, 0);
	if(data) // exists
	{
		GLenum format;
		if(nComponents == 1)
			format = GL_RED;
		else if(nComponents == 3)
			format = GL_RGB;
		else if(nComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
	}

	return textureID;
}