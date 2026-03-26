#version 330 core
/* Input vertex attributes and the position they hace in the vertex array*/
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

/* Output variable */
out vec2 texCoord;

uniform mat4 transform;

void main() {
	gl_Position = transform * vec4(aPos, 1.0);
	texCoord = vec2(aTexCoord.x, aTexCoord.y);
}
