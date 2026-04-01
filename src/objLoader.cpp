#define OBJ_LOADER_IMPLEMENTATION
#include <objLoader.hpp>

#include <fstream>
#include <sstream>
#include <map>

using namespace std;

vector<string> splitLine(const string &line);
array<int, 2> splitFace(const string &face);

void readVertex(const string &objFile, vector<Vertex> &outVertices, vector<array<int, 3>> &outFaces) {
	/* Open file */
	ifstream file(objFile);
	if(!file.is_open()) {
		printf("Failed to read obj file");
		return;
	}

	string s;
	vector<array<float, 3>> vertices;
	vector<array<float, 2>> texVertices;
	map<array<int, 2>, int> vertexIndexPairs;

	while(getline(file, s)) {
		vector<string> splitted = splitLine(s);

		if(splitted.empty()) continue;

		string id = splitted[0];	/* v,vt,f... */

		if(id == "v") {	/* v x y z */
			vertices.push_back({ stof(splitted[1]),
								 stof(splitted[2]),
								 stof(splitted[3]) });
		}
		else if(id == "vt") { /* v x y */
			texVertices.push_back({ stof(splitted[1]),
								    stof(splitted[2]) });
		}
		else if(id == "f") { /* f v/vt/vn v/vt/vn v/vt/vn */
			array<int, 3> face;
			for(unsigned int i = 0; i < 3; i++) {
				array<int, 2> pair = splitFace(splitted[i + 1]);

				if(vertexIndexPairs.find(pair) == vertexIndexPairs.end()) {
					Vertex vert;
					vert.v = vertices[pair[0] - 1];
					vert.vt = texVertices[pair[1] - 1];
					vertexIndexPairs[pair] = outVertices.size();
					outVertices.push_back(vert);
				}

				face[i] = vertexIndexPairs[pair];
			}
			outFaces.push_back(face);
		}
	}
}

vector<string> splitLine(const string &line) {
	vector<string> tokens;
	istringstream iss(line);
	string token;

	while(iss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

array<int, 2> splitFace(const string &face) {
	size_t vDiv = face.find('/');
	int vIdx = stoi(face.substr(0, vDiv));
	int vtIdx;

	if(vDiv != string::npos) {
		size_t vtDiv = face.find('/', vDiv + 1);
		string vtStr = face.substr(vDiv + 1, vtDiv - vDiv - 1); /* Ignore the normal vector */
		if(!vtStr.empty()) vtIdx = stoi(vtStr);
	}
	
	return { vIdx, vtIdx };
}