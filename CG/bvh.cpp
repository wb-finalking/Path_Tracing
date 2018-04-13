#include "bvh.h"

//***********BBox*****************//
BBox::BBox() :minxyz(Vector3f(-EPSILON, -EPSILON, -EPSILON)), maxxyz(Vector3f(EPSILON, EPSILON, EPSILON)){}

BBox::BBox(const BBox& bb)
{
	minxyz = bb.minxyz;
	maxxyz = bb.maxxyz;
}

BBox::BBox(Vector3f _minxyz, Vector3f _maxxyz)
{
	minxyz[0] = _minxyz[0] - EPSILON;
	minxyz[1] = _minxyz[1] - EPSILON;
	minxyz[2] = _minxyz[2] - EPSILON;

	maxxyz[0] = _maxxyz[0] + EPSILON;
	maxxyz[1] = _maxxyz[1] + EPSILON;
	maxxyz[2] = _maxxyz[2] + EPSILON;
}

BBox::~BBox()
{
}

void BBox::initBBox(const BBox& bb)
{
	minxyz = bb.minxyz;
	maxxyz = bb.maxxyz;
}

void BBox::initBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3)
{
	float minx, miny, minz, maxx, maxy, maxz;

	minx = v1[0] > v2[0] ? v2[0] : v1[0];
	minx = minx > v3[0] ? v3[0] : minx;
	maxx = v1[0] < v2[0] ? v2[0] : v1[0];
	maxx = maxx < v3[0] ? v3[0] : maxx;

	miny = v1[1] > v2[1] ? v2[1] : v1[1];
	miny = miny > v3[1] ? v3[1] : miny;
	maxy = v1[1] < v2[1] ? v2[1] : v1[1];
	maxy = maxy < v3[1] ? v3[1] : maxy;

	minz = v1[2] > v2[2] ? v2[2] : v1[2];
	minz = minz > v3[2] ? v3[2] : minz;
	maxz = v1[2] < v2[2] ? v2[2] : v1[2];
	maxz = maxz < v3[2] ? v3[2] : maxz;

	minxyz[0] = minx;
	minxyz[1] = miny;
	minxyz[2] = minz;

	maxxyz[0] = maxx;
	maxxyz[1] = maxy;
	maxxyz[2] = maxz;
}

void BBox::extendBBox(const BBox& bb)
{
	minxyz[0] = minxyz[0] > bb.minxyz[0] ? bb.minxyz[0] : minxyz[0];
	minxyz[1] = minxyz[1] > bb.minxyz[1] ? bb.minxyz[1] : minxyz[1];
	minxyz[2] = minxyz[2] > bb.minxyz[2] ? bb.minxyz[2] : minxyz[2];

	maxxyz[0] = maxxyz[0] < bb.maxxyz[0] ? bb.maxxyz[0] : maxxyz[0];
	maxxyz[1] = maxxyz[1] < bb.maxxyz[1] ? bb.maxxyz[1] : maxxyz[1];
	maxxyz[2] = maxxyz[2] < bb.maxxyz[2] ? bb.maxxyz[2] : maxxyz[2];
}

void BBox::extendBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3)
{
	float minx, miny, minz, maxx, maxy, maxz;

	minx = v1[0] > v2[0] ? v2[0] : v1[0];
	minx = minx > v3[0] ? v3[0] : minx;
	maxx = v1[0] < v2[0] ? v2[0] : v1[0];
	maxx = maxx < v3[0] ? v3[0] : maxx;

	miny = v1[1] > v2[1] ? v2[1] : v1[1];
	miny = miny > v3[1] ? v3[1] : miny;
	maxy = v1[1] < v2[1] ? v2[1] : v1[1];
	maxy = maxy < v3[1] ? v3[1] : maxy;

	minz = v1[2] > v2[2] ? v2[2] : v1[2];
	minz = minz > v3[2] ? v3[2] : minz;
	maxz = v1[2] < v2[2] ? v2[2] : v1[2];
	maxz = maxz < v3[2] ? v3[2] : maxz;

	minxyz[0] = minxyz[0] > minx ? minx : minxyz[0];
	minxyz[1] = minxyz[1] > miny ? miny : minxyz[1];
	minxyz[2] = minxyz[2] > minz ? minz : minxyz[2];

	maxxyz[0] = maxxyz[0] < maxx ? maxx : maxxyz[0];
	maxxyz[1] = maxxyz[1] < maxy ? maxy : maxxyz[1];
	maxxyz[2] = maxxyz[2] < maxz ? maxz : maxxyz[2];
}

int BBox::maxDimension()
{
	Vector3f range = maxxyz - minxyz;

	int result = 0;
	if (range[1] > range[0])
		result = 1;
	if (range[2] > range[1])
		result = 2;
	return result;
}

float BBox::surfaceArea()
{
	Vector3f range = maxxyz - minxyz;
	return 2*(range[0]*range[2] + range[0]*range[1] + range[1]*range[2]);
}

bool BBox::intersect(const Vector3f& org, const Vector3f& dir)
{
	Vector2f t;
	return intersect(org,dir,t);
}

bool BBox::intersect(const Vector3f& org, const Vector3f& dir, Vector2f &t)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (dir[0] >= 0) {
		tmin = (minxyz[0] - org[0]) / dir[0];
		tmax = (maxxyz[0] - org[0]) / dir[0];
	}
	else {
		tmin = (maxxyz[0] - org[0]) / dir[0];
		tmax = (minxyz[0] - org[0]) / dir[0];
	}
	if (dir[1] >= 0) {
		tymin = (minxyz[1] - org[1]) / dir[1];
		tymax = (maxxyz[1] - org[1]) / dir[1];
	}
	else {
		tymin = (maxxyz[1] - org[1]) / dir[1];
		tymax = (minxyz[1] - org[1]) / dir[1];
	}
	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	if (dir[2] >= 0) {
		tzmin = (minxyz[2] - org[2]) / dir[2];
		tzmax = (maxxyz[2] - org[2]) / dir[2];
	}
	else {
		tzmin = (maxxyz[2] - org[2]) / dir[2];
		tzmax = (minxyz[2] - org[2]) / dir[2];
	}
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	t[0] = tmin;
	t[1] = tmax;
	//return ((tmin < t1) && (tmax > 0));
	return true;
}

//***********Primitive*****************//
Primitive::Primitive(){}

Primitive::Primitive(int _face_id) : face_id(_face_id){}

void Primitive::initBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3)
{
	bbox.initBBox(v1, v2, v3);
}

void Primitive::updateBBox(Vector3f &v1, Vector3f &v2, Vector3f &v3)
{
	bbox.extendBBox(v1, v2, v3);
}

//***********BVH*****************//
BVH::BVH(std::vector<Face>*_faces, std::vector<Vertex>*_vertices, int _leaf_prim_num) : root(NULL), faces(_faces), vertices(_vertices),
	leaf_prim_num(_leaf_prim_num), max_depth(8)
{
	primitives.clear();
	primitives.reserve((*_faces).size());
	for (int i = 0; i < (*_faces).size(); i++)
	{
		Primitive tmp(i);
		Vector3f v1 = (*_vertices)[(*_faces)[i].verts[0]].coordinate;
		Vector3f v2 = (*_vertices)[(*_faces)[i].verts[1]].coordinate;
		Vector3f v3 = (*_vertices)[(*_faces)[i].verts[2]].coordinate;
		tmp.center = (v1 + v2 + v3) / 3;
		tmp.initBBox(v1, v2, v3);
		primitives.push_back(tmp);
	}
}

BVH::~BVH()
{
	delete root;
}

void BVH::build(BVHNode* root, int start, int prims_num)
{
	root->start = start;
	root->nPrims = prims_num;
	root->is_leaf = true;
	root->bbox.initBBox(primitives[start].bbox);
	for (int i = 1; i < prims_num; i++)
	{
		root->bbox.extendBBox(primitives[start+i].bbox);
	}

	if (prims_num > leaf_prim_num&&root->depth<max_depth)
	{
		root->is_leaf = false;
		int split_dim = root->bbox.maxDimension();
		float split_coord = (root->bbox.minxyz[split_dim] + root->bbox.maxxyz[split_dim]) / 2;

		int mid = start;
		for (int i = 0; i<prims_num; ++i)
		{
			if (primitives[start + i].center[split_dim] < split_coord) 
			{
				std::swap(primitives[start + i], primitives[mid]);
				++mid;
			}
		}

		// If we get a bad split, just choose the center...
		if (mid == start || mid ==(start+prims_num))
		{
			mid = start + (prims_num-1) / 2;
		}

		root->l_node = new BVHNode;
		root->r_node = new BVHNode;
		root->l_node->depth = root->depth + 1;
		root->r_node->depth = root->depth + 1;
		build(root->l_node, start, mid-start);
		build(root->r_node, mid, prims_num - mid + start);
	}
}

bool BVH::intersecting(const Vector3f& org, const Vector3f& dir, float &t, Vector3f& normal)
{
	if (root->bbox.intersect(org, dir))
		return iterateIntersect(root, org, dir, t, normal);
	else
		return false;
}

bool BVH::iterateIntersect(BVHNode* root, const Vector3f& org, const Vector3f& dir, float &t, Vector3f& normal)
{
	if (root->is_leaf)
	{
		float mint = -1;
		int start = root->start;
		for (int i = 0; i < root->nPrims; i++)
		{
			float tmp_t;
			Vector3f tmp_normal;
			int face_id = primitives[start + i].face_id;
			if (intersectionTriangle(org, dir, face_id, tmp_t, tmp_normal))
				if (tmp_t < mint||mint==-1)
				{
					mint = tmp_t;
					normal = tmp_normal;
				}	
		}
		if (mint > 0)
		{
			t = mint;
			return true;
		}
		else
			return false;
	}
	else
	{
		Vector2f l_t, r_t;
		bool l_hitc = root->l_node->bbox.intersect(org, dir, l_t);
		bool r_hitc = root->r_node->bbox.intersect(org, dir, r_t);
		if (l_hitc && r_hitc)
		{
			// We assume that the left child is a closer hit...
			BVHNode *near = root->l_node;
			BVHNode *far = root->r_node;

			// ... If the right child was actually closer, swap the relavent values.
			float t1 = l_t[0] > 0 ? l_t[0] : l_t[1];
			float t2 = r_t[0] > 0 ? r_t[0] : r_t[1];
			//if (t2 < t1) {
			//	std::swap(near, far);
			//}

			// It's possible that the nearest object is still not intersected, but we'll
			// check the further-away node later...
			Vector3f normal1, normal2;
			bool hit1 = iterateIntersect(near, org, dir, t1, normal1);
			bool hit2 = iterateIntersect(far, org, dir, t2, normal2);

			//if (hit1&&!l_hitc)
			//{
			//	Vector3f tmp = org + t1*dir;
			//	std::cout << "-------------------" << std::endl;
			//	std::cout << "t---" << t1 << std::endl;
			//	std::cout << root->l_node->bbox.minxyz[0] << "***" << tmp[0] << "***" << root->l_node->bbox.maxxyz[0] << std::endl;
			//	std::cout << root->l_node->bbox.minxyz[1] << "***" << tmp[1] << "***" << root->l_node->bbox.maxxyz[1] << std::endl;
			//	std::cout << root->l_node->bbox.minxyz[2] << "***" << tmp[2] << "***" << root->l_node->bbox.maxxyz[2] << std::endl;

			//	for (int index = 0; index < root->l_node->nPrims; index++)
			//	{
			//		std::cout << "-------------------" << std::endl;
			//		int face_id = primitives[root->l_node->start + index].face_id;
			//		int v1 = (*faces)[face_id].verts[0];
			//		int v2 = (*faces)[face_id].verts[1];
			//		int v3 = (*faces)[face_id].verts[2];
			//		std::cout << (*vertices)[v1].coordinate[0] << "***" << (*vertices)[v1].coordinate[1] << "***" << (*vertices)[v1].coordinate[2] << std::endl;
			//		std::cout << (*vertices)[v2].coordinate[0] << "***" << (*vertices)[v2].coordinate[1] << "***" << (*vertices)[v2].coordinate[2] << std::endl;
			//		std::cout << (*vertices)[v3].coordinate[0] << "***" << (*vertices)[v3].coordinate[1] << "***" << (*vertices)[v3].coordinate[2] << std::endl;

			//		Vector3f E1 = (*vertices)[v2].coordinate - (*vertices)[v1].coordinate;
			//		Vector3f E2 = (*vertices)[v3].coordinate - (*vertices)[v1].coordinate;
			//		Vector3f D = dir;
			//		Vector3f T = org - (*vertices)[v1].coordinate;
			//		Matrix3f M, M1, M2, M3;
			//		M.col(0) = -D;
			//		M.col(1) = E1;
			//		M.col(2) = E2;
			//		M1 = M;
			//		M1.col(0) = T;
			//		M2 = M;
			//		M2.col(1) = T;
			//		M3 = M;
			//		M3.col(2) = T;
			//		float det_M = M.determinant();
			//		float det_M1 = M1.determinant();
			//		float det_M2 = M2.determinant();
			//		float det_M3 = M3.determinant();
			//		if (std::fabs(det_M - 0) < 0.00001)
			//			continue;
			//		float tt = det_M1 / det_M;
			//		float u = det_M2 / det_M;
			//		float v = det_M3 / det_M;
			//		std::cout << "uv------" << u << "--" << v <<std::endl;
			//		std::cout << "t---" << tt << std::endl;
			//	}
			//	exit(1);
			//}

			if (hit1&&hit2)
				if (t1 < t2)
				{
					t = t1;
					normal = normal1;
					return true;
				}
				else
				{
					t = t2;
					normal = normal2;
					return true;
				}
			else if (hit1)
			{
				t = t1;
				normal = normal1;
				return true;
			}
			else if (hit2)
			{
				t = t2;
				normal = normal2;
				return true;
			}
		}
		else if (l_hitc) {
			return iterateIntersect(root->l_node, org, dir, t, normal);
		}
		else if (r_hitc) {
			return iterateIntersect(root->r_node, org, dir, t, normal);
		}
		return false;
	}
		
}

bool BVH::intersectionTriangle(const Vector3f& org, const Vector3f& dir, int face_id, float &t, Vector3f& normal)
{
	Vector3f E1 = (*vertices)[(*faces)[face_id].verts[1]].coordinate - (*vertices)[(*faces)[face_id].verts[0]].coordinate;
	Vector3f E2 = (*vertices)[(*faces)[face_id].verts[2]].coordinate - (*vertices)[(*faces)[face_id].verts[0]].coordinate;
	Vector3f D = dir;
	Vector3f T = org - (*vertices)[(*faces)[face_id].verts[0]].coordinate;

	Matrix3f M, M1, M2, M3;
	M.col(0) = -D;
	M.col(1) = E1;
	M.col(2) = E2;

	M1 = M;
	M1.col(0) = T;

	M2 = M;
	M2.col(1) = T;

	M3 = M;
	M3.col(2) = T;

	float det_M = M.determinant();
	float det_M1 = M1.determinant();
	float det_M2 = M2.determinant();
	float det_M3 = M3.determinant();

	if (std::fabs(det_M - 0) < 0.00001)
		return false;

	t = det_M1 / det_M;

	float u = det_M2 / det_M;
	float v = det_M3 / det_M;
	if (u >= 0 && v >= 0 && u + v <= 1.0f&&t > 0)
	{
		normal = E1.cross(E2).normalized();
		return true;
	}
	return false;
}