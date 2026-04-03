#define OBJ_LOADER_IMPLEMENTATION
#include <objLoader.hpp>

#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

void readObj(const string &objFile, vector<Vertex> &outVertices, vector<array<int, 3>> &outFaces);
vector<string> splitLine(const string &line);
array<int, 2> splitFace(const string &face);

struct PairHash { /* Custom hash for the map used in readObj */
	size_t operator()(const array<int, 2> &p) const {
		return hash<int>()(p[0]) ^ (hash<int>()(p[1]) << 16);
	}
};

void readObj(const string &objFile, vector<Vertex> &outVertices, vector<array<int, 3>> &outFaces) {
	/* Open file */
	ifstream file(objFile);
	if(!file.is_open()) {
		cerr << "Failed to read obj file" << endl;
		return;
	}

	string s;
	vector<array<float, 3>> vertices;
	vector<array<float, 2>> texVertices;
	unordered_map<array<int, 2>, int, PairHash> vertexIndexPairs;

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
				/* Read the vertex index if is already saved */
				auto [it, hadToInsert] = vertexIndexPairs.try_emplace(pair, (int) outVertices.size());
				if(hadToInsert) {
					/* If the vertex had to be inserted in the map, we added to the list*/
					Vertex vert;
					vert.v = vertices[pair[0] - 1];
					vert.vt = texVertices[pair[1] - 1];
					outVertices.push_back(vert);
				}
				/* Save the index for the face */
				face[i] = it -> second;
			}
			outFaces.push_back(face);
		}
	}
}

vector<string> splitLine(const string &line) {
	vector<string> tokens;
	const char *p = line.c_str();
	while(*p) {
		/* Skip whitespace */
		while(*p == ' ' || *p == '\t') p++;
		if(!*p) break;
		/* Get first token */
		const char *start = p;
		/* Skip more whitespace */
		while(*p && *p != ' ' && *p != '\t') p++;

		tokens.emplace_back(start, p);
	}
	return tokens;
}

array<int, 2> splitFace(const string &face) {
	size_t vDiv = face.find('/');
	int vIdx = 0, vtIdx = 0;
	vIdx = stoi(face.substr(0, vDiv));

	if(vDiv != string::npos) {
		size_t vtDiv = face.find('/', vDiv + 1);
		string vtStr = face.substr(vDiv + 1, vtDiv - vDiv - 1); /* Ignore the normal vector */
		if(!vtStr.empty()) vtIdx = stoi(vtStr);
	}
	
	return { vIdx, vtIdx };
}