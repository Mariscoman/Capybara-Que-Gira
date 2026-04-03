#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include <string>
#include <vector>
#include <array>

struct Vertex {
	std::array<float, 3> v;
	std::array<float, 2> vt;

	bool operator < (const Vertex &other) const {
		return (v != other.v) ? v < other.v : vt < other.vt;
	}
};

void readObj(const std::string &objFile, std::vector<Vertex> &outVertices, std::vector<std::array<int, 3>> &outFaces);

#endif