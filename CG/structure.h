#ifndef _STRUCTURE_
#define _STRUCTURE_
#include <eigen/dense>
//#include <eigen/Core>
#include <vector>

using namespace Eigen;

struct Vertex {
	Vector3f coordinate;
	Vector3f norm;
	int halfEdge;
};

struct Face {
	std::vector<int> verts;
	Vector3f norm;
};

struct HalfEdge {
	//another halfedge inedx of  this halfedge index
	int pair;
};
#endif