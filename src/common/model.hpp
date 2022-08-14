#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include <glm.hpp>

#include <string>
#include <vector>

// Auxillary structure for code readability
struct FaceElement {
	unsigned p;	// Position
	unsigned n;	// Normal
	unsigned t;	// Texture Coordinate
};

struct MeshIndexTable {
	MeshIndexTable() 
		: m(0), f(0)
	{}
	uint32_t m, f;
};

struct MeshAttributeSizes {
	MeshAttributeSizes()
		: fsize(0)
	{}
	uint32_t fsize;
};

// A model is a class that combines multiple meshes.
class Model {

public:

	Model() {}

	Model(Model&& other) noexcept {}

	void parse(const char* filename);
	
	//
	// Get base of attributes.
	glm::vec3* GetVertices() {
		return vertices.data();
	}

	glm::vec3* GetNormals() {
		return normals.data();
	}

	glm::vec2* GetUVs() {
		return uvs.data();
	}

	FaceElement* GetFaces() {
		return faces.data();
	}

	//
	// Get indiviudal attribute
	glm::vec3 GetVertex(const uint32_t vertex_index) const {
		return vertices[vertex_index];
	}

	glm::vec3 GetNormal(const uint32_t normal_index) const {
		return normals[normal_index];
	}

	glm::vec2 GetUV(const uint32_t uv_index) const {
		return uvs[uv_index];
	}

	FaceElement GetFace(const uint32_t face_index) const {
		return faces[face_index];
	}

	int GetMeshIndex(const int face_index) const {
		for (int i = 0; i < mesh_indices.size(); ++i) {
			if (	face_index >= mesh_indices[i].f 
				&&	face_index <= mesh_indices[i].f + mesh_attribute_sizes[i].fsize) 
			{
				return i;
			}
		}
		return -1;
	}

	std::string GetMeshName(const int mesh_index) const {
		return meshNames[mesh_index];
	}

	//
	// Get number of attributes
	std::size_t GetVertexCount() const {
		return vertices.size();
	}

	std::size_t GetFaceCount() const {
		return faces.size();
	}

	std::size_t GetTriangleCount() const {
		return faces.size() / 3;
	}

	std::size_t GetFaceCountOfMesh(const uint32_t mesh_index) const {
		return mesh_attribute_sizes[mesh_index].fsize;
	}

	std::size_t GetNumberOfMeshes() const {
		return nMeshes;
	}

	uint32_t GetMeshIndexFromFace(uint32_t face_index) const {
		face_index *= 3;
		for (int i = 0; i < nMeshes; ++i) {
			if (face_index < mesh_indices[i].f + mesh_attribute_sizes[i].fsize) {
				return i;
			}
		}
		return std::numeric_limits<uint32_t>::infinity();
	}

	std::vector<float> GetInterleavedAttributes();

protected:

	std::size_t nMeshes;
	std::vector<MeshAttributeSizes> mesh_attribute_sizes;
	std::vector<MeshIndexTable> mesh_indices;

	std::vector<std::string> meshNames;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	
	std::vector<glm::vec2> uvs;
	std::vector<unsigned char*> texture_data;

	std::vector<FaceElement> faces;
};

#endif