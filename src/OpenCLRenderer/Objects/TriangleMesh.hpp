#ifndef BRDFEDITOR_OPENCL_TRIANGLE_MESH_HEADER
#define BRDFEDITOR_OPENCL_TRIANGLE_MESH_HEADER

#include <vector>

#include <src/OpenCLRenderer/CLHeaders.hpp>
#include <src/OpenCLRenderer/Objects/BVH/BVH.hpp>

#include "Object.hpp"

namespace brdfEditor::clRenderer {
struct Triangle {
	glm::vec3 v0, v1, v2;
};

// TODO Apply Transform?
class TriangleMesh : public Object {
public:
	struct TriangleMesh_cl {
		// Indices to triangles array, mesh consists of [begin,end) range.
		cl_int beginTri, endTri;
		// Index of BVH root node in nodes array.
		cl_int rootIndex;
	};
	using Triangles = std::vector<Triangle>;

	TriangleMesh(const std::string &name, const MaterialCHandle &mat,
				 Triangles &&tris);
	// Copies triangles into passed buffers and returns OpenCL mesh
	// representation.
	TriangleMesh_cl
	getCL(BVH::clTris &tris, BVH::clNodes &nodes) const;
	// Loads .obj model from a file.
	// --filename: Including path and extension.
	// --position: Translated the model to this position.
	// Returns model's triangles.
	static Triangles
	loadModel(const char *filename,glm::vec3 position);

private:
	BVH bvh;
};
} // namespace brdfEditor::clRenderer
#endif // !BRDFEDITOR_OPENCL_TRIANGLE_MESH_HEADER
