#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
}

void keyPressCallback(GLFWwindow *window, int key, int, int action, int) {
	static bool drawingMode = false;

	/* Check key presses */
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		drawingMode = !drawingMode;
		glPolygonMode(GL_FRONT_AND_BACK, drawingMode ? GL_LINE : GL_FILL);
	}
}

#endif