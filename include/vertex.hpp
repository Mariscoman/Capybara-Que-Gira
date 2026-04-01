struct Vertex {
	float position[3];
	float texCoord[2];

	bool operator==(const Vertex &other) const {
		return position[0] == other.position[0] &&
               position[1] == other.position[1] &&
               position[2] == other.position[2] &&
               texCoord[0] == other.texCoord[0] &&
               texCoord[1] == other.texCoord[1];
	}
}