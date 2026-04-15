#include <iostream>
#include <math.h>
#include <vector>
#include <array>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image/stb_image.h>
#include <miniaudio/miniaudio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <callbacks.hpp>
#include <shader.hpp>
#include <objLoader.hpp>

unsigned int readTexture(const char *textureRoute, unsigned int format = GL_RGB);
ma_result playMusic(ma_engine *engine, ma_sound *sound, const char *file);

int main() {

	std::cout << "Capybara: Initializing environment" << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	/* Creating the window */
	GLFWwindow *window = glfwCreateWindow(800, 600, "Capybara Que Gira", NULL, NULL);
	if(window == NULL) {
		std::cout << "Capybara: Failed window creation" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	/* Initializing glad */
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Capybara: Failed GLAD initialization" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	/* Creating and compiling shaders */
	std::cout << "Capybara: Compiling shaders" << std::endl;
	Shader shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");

	/* Reading obj file */
	std::cout << "Capybara: Loading model" << std::endl;
	std::vector<Vertex> vertices;
	std::vector<std::array<int, 3>> faces;
	readObj("resources/models/capybaraLowerPoly.obj", vertices, faces);

	/* Creating vertex and element buffers */
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	/* Setting vertex and face data */
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(std::array<int, 3>), faces.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	/* Creating texture */
	std::cout << "Capybara: Loading texture" << std::endl;
	unsigned int texture = readTexture("resources/textures/capybara.jpeg", GL_RGB);

	shader.use();
	shader.setInt("texture", 0);

	/* Loading music */
	std::cout << "Capybara: Loading music" << std::endl;
	ma_engine engine;
	ma_sound sound;
	if(playMusic(&engine, &sound, "resources/music/StayInsideMe.mp3") != MA_SUCCESS) {
		std::cout << "Capybara: Failed loading music" << std::endl;
	}

	/* RNG */
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distr(0.0, 1.0);

	/* First color and color switch time */
	float r = distr(gen),
		  g = distr(gen),
		  b = distr(gen);
	float lastColorChange = 0.0f,
		  colorInterval = 0.5f;

	std::cout << "Capybara: Capybara" << std::endl;
	ma_sound_start(&sound);
	glfwShowWindow(window);

	while(!glfwWindowShouldClose(window)) {
		/* Change backgorund color */
		float currTime = glfwGetTime();
		if(currTime - lastColorChange >= colorInterval) {
			r = distr(gen);
			g = distr(gen);
			b = distr(gen);
			lastColorChange = currTime;
		}
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		/* Rotate object */
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, currTime + 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		
		/* Render object */
		shader.use();
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

	ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);

	glfwTerminate();
	return 0;
}

unsigned int readTexture(const char *textureRoute, unsigned int format) {
	unsigned int ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(textureRoute, &width, &height, &nrChannels, 0);
	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Capybara: Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return ID;
}

ma_result playMusic(ma_engine *engine, ma_sound *sound, const char *file) {
	ma_result result;

	result = ma_engine_init(NULL, engine);
	if(result != MA_SUCCESS) return result;

	/* Initialize sound from file */
	result = ma_sound_init_from_file(engine, file, MA_SOUND_FLAG_DECODE, NULL, NULL, sound);
	if(result != MA_SUCCESS) {
		ma_engine_uninit(engine);
		return result;
	}

	ma_sound_set_looping(sound, MA_TRUE);

	return MA_SUCCESS;
}