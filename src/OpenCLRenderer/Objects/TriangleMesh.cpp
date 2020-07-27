#include "TriangleMesh.hpp"

#include <external/tiny_obj_loader/tiny_obj_loader.h>

namespace brdfEditor::clRenderer {
TriangleMesh::TriangleMesh(const std::string &name, const MaterialCHandle &mat,
						   Triangles &&tris) :
	Object(name, mat),
	bvh(std::move(tris)) {}

TriangleMesh::TriangleMesh_cl
TriangleMesh::getCL(BVH::clTris &tris, BVH::clNodes &nodes) const {
	TriangleMesh_cl cl;
	cl.beginTri = static_cast<cl_int>(tris.size());
	cl.rootIndex = bvh.getCL(nodes, tris);
	cl.endTri = static_cast<cl_int>(cl.beginTri + tris.size());
	return cl;
}
TriangleMesh::Triangles
TriangleMesh::loadModel(const char *filename,glm::vec3 position) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	using namespace clRenderer;
	bool ret =
		tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);
	TriangleMesh::Triangles triangles;
	if (!ret)
		triangles;

	assert(shapes.size() == 1);
	for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); ++i) {
		Triangle t;
		auto index = shapes[0].mesh.indices[3 * i + 0];
		;
		t.v0.x = attrib.vertices[3 * index.vertex_index + 0];
		t.v0.y = attrib.vertices[3 * index.vertex_index + 1] - 0.5f;
		t.v0.z = attrib.vertices[3 * index.vertex_index + 2];
		index = shapes[0].mesh.indices[3 * i + 1];
		;
		t.v1.x = attrib.vertices[3 * index.vertex_index + 0];
		t.v1.y = attrib.vertices[3 * index.vertex_index + 1] - 0.5f;
		t.v1.z = attrib.vertices[3 * index.vertex_index + 2];

		index = shapes[0].mesh.indices[3 * i + 2];
		;
		t.v2.x = attrib.vertices[3 * index.vertex_index + 0];
		t.v2.y = attrib.vertices[3 * index.vertex_index + 1] - .5f;
		t.v2.z = attrib.vertices[3 * index.vertex_index + 2];
		t.v0 += position;
		t.v1 += position;
		t.v2 += position;
		triangles.push_back(t);
	}

	return triangles;
}
} // namespace brdfEditor::clRenderer
