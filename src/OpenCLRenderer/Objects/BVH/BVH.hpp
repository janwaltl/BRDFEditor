#ifndef BRDFEDITOR_OPENCL_RENDERER_BVH_HEADER
#define BRDFEDITOR_OPENCL_RENDERER_BVH_HEADER

#include <vector>
#include <glm/common.hpp>

#include <src/OpenCLRenderer/CLHeaders.hpp>

namespace brdfEditor::clRenderer {
struct Triangle;

// Axis-aligned bounding box.
struct AABB {
	glm::vec3 min, max;

	// Returns an AABB with max<min and zero surface area.
	static AABB
	empty();

	void
	unionWith(const AABB &other);
	// Returns 0.0f for empty or unbounded AABBs.
	double
	surfaceArea() const;
};

// Bounding volume hiearchy of given set of triangles.
class BVH {
public:
	struct alignas(64) Node {
		std::vector<std::size_t> tIDs;
		AABB bounds;
		std::unique_ptr<std::pair<Node, Node>> children;
	};

	struct alignas(64) Node_cl {
		// Negative children[0] means numTriangles and in that case children[1]
		// is the index of the first triangle
		cl_int children[2];
		// min,max for the first child, min max for the second
		cl_float childBounds[12];
	};

	struct alignas(64) Triangle_cl {
		// .w are not currently used. Done for structure alignment and
		// size(float3)==size(float4).
		cl_float4 v0, e1, e2, n;
	};

	using clTris = std::vector<Triangle_cl>;
	using clNodes = std::vector<Node_cl>;

	BVH(std::vector<Triangle> &&triangles);
	// Adds BVHnodes and triangles from this BVH to the buffers.
	// Returns index of the root node in the nodes buffer.
	cl_int
	getCL(clNodes &nodes, clTris &tris) const;

private:
	Node root;
	std::vector<Triangle> triangles;
};
} // namespace brdfEditor::clRenderer
#endif
