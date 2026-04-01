#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
}

#endif