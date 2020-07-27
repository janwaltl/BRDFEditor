#include "BVH.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>

#include <glm/glm.hpp>

#include <src/OpenCLRenderer/Objects/TriangleMesh.hpp>

namespace brdfEditor::clRenderer {
namespace {
// Implmentation of https://graphics.stanford.edu/~boulos/papers/togbvh.pdf

	constexpr const double Taabb = 1.0;
constexpr const double Ttri = 1.5;
static std::size_t internalNodes = 0;
static std::size_t leafNodes = 0;
static std::size_t leafTris = 0;

struct TriStruct {
	AABB aabb;
	glm::vec3 centroid;
	std::size_t tIndex;
	char paddTo64[12];
};
struct SetArea {
	double leftArea, rightArea;
};

using TriStructs_t = std::vector<TriStruct>;
using TrisIT = TriStructs_t::iterator;


AABB
computeTriBounds(const Triangle &t) {
	AABB aabb;
	aabb.min = glm::min(t.v0, glm::min(t.v1, t.v2));
	aabb.max = glm::max(t.v0, glm::max(t.v1, t.v2));
	return aabb;
}
TriStructs_t
computeTriStructs(const std::vector<Triangle> &triangles) {
	TriStructs_t triStructs(triangles.size());

	for (std::size_t i = 0; i < triangles.size(); ++i) {
		const Triangle &t = triangles[i];
		TriStruct &triStr = triStructs[i];
		triStr.aabb = computeTriBounds(t);
		triStr.centroid = (t.v0 + t.v1 + t.v2) / 3.0f;
		triStr.tIndex = i;
	}
	return triStructs;
}
// Sorts triangle structs according to their centroid positions for given
// axis(x=0,y=1,z=2). Range is [b,e).
void
sortTriStructs(TrisIT b, TrisIT e, int axis) {
	std::sort(b, e, [axis = axis](const TriStruct &l, const TriStruct &r) {
		return l.centroid[axis] < r.centroid[axis];
	});
}

// Makes and returns a leaf node that contains triangles from [b,e) range.
BVH::Node
makeLeaf(TrisIT b, TrisIT e, const std::vector<Triangle> &triangles) {
	++leafNodes;
	std::size_t setSize = e - b;
	leafTris += setSize;
	//assert(setSize < 4);
	BVH::Node leaf;
	leaf.tIDs.reserve(setSize);
	leaf.bounds = AABB::empty();
	for (std::size_t i = 0; i < setSize; ++i) {
		auto tID = (b++)->tIndex;
		leaf.bounds.unionWith(computeTriBounds(triangles[tID]));
		leaf.tIDs.push_back(tID);
	}
	return leaf;
}
// Makes and return an internal node containg two passed children.
BVH::Node
makeInternal(BVH::Node &&leftChild, BVH::Node &&rightChild,
			 const std::vector<Triangle> &triangles) {
	using p = std::pair<BVH::Node, BVH::Node>;

	++internalNodes;
	BVH::Node internal;
	internal.bounds = leftChild.bounds;
	internal.bounds.unionWith(rightChild.bounds);
	internal.children = std::make_unique<p>(
		std::move(leftChild), std::move(rightChild));
	return internal;
}
namespace {
	static std::size_t maxDepth = 1;
}
BVH::Node
build(TrisIT b, TrisIT e, const std::vector<Triangle> &triangles,std::size_t depth) {
	maxDepth = std::max(depth, maxDepth);
	std::size_t setSize = e - b;
	if (setSize == 1)
		return makeLeaf(b, e, triangles);

	double bestCost = setSize * Ttri;
	int bestAxis = -1;
	std::size_t bestSplit = std::numeric_limits<std::size_t>::max();

	for (int axis = 0; axis < 3; ++axis) {
		sortTriStructs(b, e, axis);

		std::vector<SetArea> splittedAreas(setSize - 1);

		AABB leftBound = AABB::empty();

		// Split n is between nth and n+1th triangle
		// Accumulate left areas
		for (std::size_t split = 0; split < setSize - 1; ++split) {
			leftBound.unionWith((b + split)->aabb);
			splittedAreas[split].leftArea = leftBound.surfaceArea();
		}

		leftBound.unionWith((b + setSize - 1)->aabb);
		double setArea = leftBound.surfaceArea();

		AABB rightBound = AABB::empty();
		// Accumulate right areas and compute the cost of splits
		std::size_t split = setSize - 2;
		do {
			rightBound.unionWith((b + split + 1)->aabb);
			splittedAreas[split].rightArea = rightBound.surfaceArea();

			double thisCost = splittedAreas[split].leftArea * (split) +
							  splittedAreas[split].rightArea *
								  (setSize - 1 - split);
			thisCost = thisCost * (Ttri / setArea) + 2 * Taabb;
			if (thisCost < bestCost) {
				bestCost = thisCost;
				bestAxis = axis;
				bestSplit = split;
			}
		} while (split-- != 0);
	}

	if (bestAxis == -1) {
		return makeLeaf(b, e, triangles);
	}
	else {
		sortTriStructs(b, e, bestAxis);
		auto &&leftCh = build(b, b + bestSplit + 1, triangles, depth+1);
		auto &&rightCh = build(b + bestSplit + 1, e, triangles, depth+1);

		return makeInternal(std::move(leftCh), std::move(rightCh), triangles);
	}
}

void
SetChildBounds(const BVH::Node *node, BVH::Node_cl &clNode) {
	AABB &fBounds = node->children->first.bounds;
	AABB &sBounds = node->children->second.bounds;

	clNode.childBounds[0] = fBounds.min[0];
	clNode.childBounds[1] = fBounds.min[1];
	clNode.childBounds[2] = fBounds.min[2];
	clNode.childBounds[3] = fBounds.max[0];
	clNode.childBounds[4] = fBounds.max[1];
	clNode.childBounds[5] = fBounds.max[2];
	clNode.childBounds[6] = sBounds.min[0];
	clNode.childBounds[7] = sBounds.min[1];
	clNode.childBounds[8] = sBounds.min[2];
	clNode.childBounds[9] = sBounds.max[0];
	clNode.childBounds[10] = sBounds.max[1];
	clNode.childBounds[11] = sBounds.max[2];
}
void
WriteTriangles(const BVH::Node *node, std::vector<BVH::Triangle_cl> &tris,
			   const std::vector<Triangle> &triangles) {

	for (auto &&tID : node->tIDs) {
		const Triangle &t = triangles[tID];

		assert(t.v0.x <= node->bounds.max.x);
		assert(t.v0.y <= node->bounds.max.y);
		assert(t.v0.z <= node->bounds.max.z);
		assert(t.v1.x <= node->bounds.max.x);
		assert(t.v1.y <= node->bounds.max.y);
		assert(t.v1.z <= node->bounds.max.z);
		assert(t.v2.x <= node->bounds.max.x);
		assert(t.v2.y <= node->bounds.max.y);
		assert(t.v2.z <= node->bounds.max.z);
		assert(t.v0.x >= node->bounds.min.x);
		assert(t.v0.y >= node->bounds.min.y);
		assert(t.v0.z >= node->bounds.min.z);
		assert(t.v1.x >= node->bounds.min.x);
		assert(t.v1.y >= node->bounds.min.y);
		assert(t.v1.z >= node->bounds.min.z);
		assert(t.v2.x >= node->bounds.min.x);
		assert(t.v2.y >= node->bounds.min.y);
		assert(t.v2.z >= node->bounds.min.z);
		BVH::Triangle_cl clT;

		clT.v0.x = t.v0.x;
		clT.v0.y = t.v0.y;
		clT.v0.z = t.v0.z;

		auto e1 = t.v1 - t.v0;
		clT.e1.x = e1.x;
		clT.e1.y = e1.y;
		clT.e1.z = e1.z;

		auto e2 = t.v2 - t.v0;
		clT.e2.x = e2.x;
		clT.e2.y = e2.y;
		clT.e2.z = e2.z;

		auto n = glm::normalize(glm::cross(e1, e2));

		clT.n.x = n.x;
		clT.n.y = n.y;
		clT.n.z = n.z;
		tris.push_back(clT);
	}
}

} // namespace

AABB
AABB::empty() {
	AABB b;
	b.min = glm::vec3(std::numeric_limits<float>::max());
	b.max = -b.min;
	return b;
}
void
AABB::unionWith(const AABB &other) {
	min = glm::min(min, other.min);
	max = glm::max(max, other.max);
}
double
AABB::surfaceArea() const {
	auto ext = max - min;

	if (glm::any(glm::isinf(ext) | glm::isnan(ext)))
		return 0;
	else
		return 2 * ext.x * ext.y + 2 * ext.y * ext.z + 2 * ext.x * ext.z;
}
BVH::BVH(std::vector<Triangle> &&triangles) : triangles(std::move(triangles)) {
	auto start = std::chrono::high_resolution_clock::now();
	TriStructs_t tristructs = computeTriStructs(this->triangles);

	root = build(tristructs.begin(), tristructs.end(), this->triangles,1);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> dur = end - start;
	std::cout << "Leaves " << leafNodes << "\tInternal" << internalNodes << '\t'
			  << leafTris << '\n';
	std::cout << "Max depth " << maxDepth << '\n';
	std::cout << "Build time " << dur.count() << '\n';
}
cl_int
BVH::getCL(clNodes &nodes, clTris &tris) const {
	std::queue<const Node *> qNodes;
	qNodes.push(&root);

	cl_int rootIndex = static_cast<cl_int>(nodes.size());
	while (!qNodes.empty()) {
		const Node *node = qNodes.front();
		qNodes.pop();

		nodes.push_back(Node_cl{});
		Node_cl &clNode = nodes.back();

		if (node->children) // Internal node
		{
			// Children will be pushed into the queue, so we know when we will
			// process them = push them into buffer. So we can calculate their
			// indices in the buffer.
			clNode.children[0] = static_cast<cl_int>(nodes.size() +
													 qNodes.size());
			clNode.children[1] = clNode.children[0] + 1;
			qNodes.push(&node->children->first);
			qNodes.push(&node->children->second);

			SetChildBounds(node, clNode);
		} else // A leaf
		{
			assert(node->tIDs.size() > 0);
			// Writes numbers of triangles in this leaf as negative == mark for
			// leaf node.
			clNode.children[0] = -static_cast<cl_int>(node->tIDs.size());
			// Index of the first triangle.
			clNode.children[1] = static_cast<cl_int>(tris.size());

			WriteTriangles(node, tris, triangles);
		}
	}
	return rootIndex;
}

} // namespace brdfEditor::clRenderer