#ifndef _BVH_
#define _BVH_
#include <iostream>
#include "structure.h"

#define EPSILON 0.00001

class BBox
{
public:
	BBox();
	BBox(const BBox& bb);
	BBox(Vector3f _minxyz, Vector3f _maxxyz);
	~BBox();
	
	void initBBox(const BBox& bb);
	void initBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3);
	void extendBBox(const BBox& bb);
	void extendBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3);
	bool intersect(const Vector3f& org, const Vector3f& dir);
	bool intersect(const Vector3f& org, const Vector3f& dir, Vector2f &t);
	int maxDimension();
	float surfaceArea();

	Vector3f minxyz, maxxyz;

};

class Primitive
{
public:
	Primitive();
	Primitive(int _face_id);

	void updateBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3);
	void initBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3);

	int face_id;
	Vector3f center;
	BBox bbox;

};

struct BVHNode
{
	BBox bbox;
	bool is_leaf;
	int start, nPrims;
	int depth;
	BVHNode *l_node, *r_node;

	BVHNode() :l_node(NULL), r_node(NULL), is_leaf(false), start(0), nPrims(0), depth(0){}
	~BVHNode()
	{
		if (l_node != NULL)
			delete l_node;
		if (r_node != NULL)
			delete r_node;
	}
};

class BVH
{
public:
	BVH(std::vector<Face>*_faces, std::vector<Vertex>*_vertices, int _leaf_prim_num);
	~BVH();

	void build(BVHNode* root, int start, int prims_num);
	bool intersecting(const Vector3f& org, const Vector3f& dir, float &t, Vector3f& normal);
	bool iterateIntersect(BVHNode* root, const Vector3f& org, const Vector3f& dir, float &t, Vector3f& normal);
	bool intersectionTriangle(const Vector3f& org, const Vector3f& dir, int face_id, float &t, Vector3f& normal);

	int leaf_prim_num;
	int max_depth;
	BVHNode* root;
	std::vector<Primitive> primitives;
	std::vector<Face>*faces;
	std::vector<Vertex>*vertices;
};



#endif