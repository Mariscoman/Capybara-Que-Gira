#include <iostream>
#include <math.h>
#include <vector>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <callbacks.hpp>
#include <shader.hpp>
#include <objLoader.hpp>

unsigned int readTexture(const char *textureRoute, unsigned int format = GL_RGB);
void readCapybara(std::string file, float *vertices, float *indices);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "Capybara Que Gira", NULL, NULL);
	if(window == NULL) {
		std::cout << "Fallo al crear la ventana" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Fallo al iniciar GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	Shader shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");

	std::vector<Vertex> vertices;
	std::vector<std::array<int, 3>> faces;
	readVertex("resources/models/CapybaraLowerPoly.obj", vertices, faces);

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(std::array<int, 3>), faces.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture = readTexture("resources/textures/capybara.jpeg", GL_RGB);

	shader.use();
	shader.setInt("texture", 0);

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, (float) glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		
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
		std::printf("Failed to load texture");
	}
	stbi_image_free(data);

	return ID;
}