#define OBJ_LOADER_IMPLEMENTATION
#include <objLoader.hpp>

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <charconv>
#include <string_view>
#include <cstring>

using namespace std;

static inline int strvToI(string_view sv);
static inline float strvToF(string_view sv);
static void splitLine(string_view line, vector<string_view> &outTokens);
static array<int, 2> splitVertexPair(string_view face);

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

	vector<string_view> splitted;

	while(getline(file, s)) {
		splitLine(s, splitted);

		if(splitted.empty()) continue;

		string_view id = splitted[0];	/* v,vt,f... */

		if(id == "v") {	/* v x y z */
			vertices.push_back({ strvToF(splitted[1]),
								 strvToF(splitted[2]),
								 strvToF(splitted[3]) });
		}
		else if(id == "vt") { /* v x y */
			texVertices.push_back({ strvToF(splitted[1]),
									strvToF(splitted[2]) });
		}
		else if(id == "f") { /* f v/vt/vn v/vt/vn v/vt/vn */
			array<int, 3> face;
			for(unsigned int i = 0; i < 3; i++) {
				array<int, 2> pair = splitVertexPair(splitted[i + 1]);
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

static inline int strvToI(string_view sv) {
	int v = 0;
	from_chars(sv.data(), sv.data() + sv.size(), v);
	return v;
}

static inline float strvToF(string_view sv) {
	float v = 0.0f;
	from_chars(sv.data(), sv.data() + sv.size(), v);
	return v;
}

static void splitLine(string_view line, vector<string_view> &outTokens) {
	outTokens.clear();
	int i = 0, n = line.size();
	while(i < n) {
		/* Skip whitespace */
		while(i < n && (line[i] == ' ' || line[i] == '\t')) i++;
		if(i == n) break;

		/* Get start of token and add it to the list */
		int tok = i;
		while(i < n && line[i] != ' ' && line[i] != '\t') i++;
		outTokens.emplace_back(line.data() + tok, i - tok);
	}
}

static array<int, 2> splitVertexPair(string_view face) {
	int vIdx = 0, vtIdx = 0;
	size_t vDiv = face.find_first_of('/');

	from_chars(face.data(), face.data() + (vDiv == string_view::npos ? face.size() : vDiv), vIdx);

	/* Read until the next / or until the end of the string */
	if(vDiv != string_view::npos) {
		string_view vt = face.substr(vDiv + 1);
		int vtDiv = vt.find_first_of('/');
		string_view vtStr = vt.substr(0, vtDiv);
		if(!vtStr.empty()) from_chars(vtStr.data(), vtStr.data() + vtStr.size(), vtIdx);
	}

	return { vIdx, vtIdx };
}