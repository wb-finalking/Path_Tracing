#ifndef _OBJECT_
#define _OBJECT_
#include <iostream>
#include <cmath>
#include "structure.h"
#include "readObj.h"
#include "Material.h"
#include "bvh.h"

class AABB
{
public:
	AABB();
	~AABB();

	float max_x, min_x;
	float max_y, min_y;
	float max_z, min_z;

	void getPlane(Vector3f plane[], int index, int fixed_v);
	bool intersection(const Vector3f org, const Vector3f& ray_dir);
	bool intersectionRectangle(const Vector3f org, const Vector3f& ray_dir, const Vector3f vVert[], float& t);

};

class Material;
class InterPoint
{
public:
	InterPoint();
	~InterPoint();

	Vector3f point;
	Vector3f normal;
	Vector3f emit_dir;
	bool is_out;//where the previous InterPoint comes from
	bool is_empty;
	float t;
	float P;
	Material *material;
};

class Object
{
public:
	Object();
	virtual ~Object()=0;
	void setMaterial(Material* m);
	virtual void getEmitPos(std::vector<InterPoint>& pos)=0;
	virtual bool intersection(const Vector3f org, const Vector3f& ray_dir, InterPoint& inter_pt , bool is_out=true)=0;

	
	Material *material;

private:
	
};

class Object_Mesh:public Object
{
public:
	Object_Mesh(std::string &filename);
	virtual ~Object_Mesh();
	Vector3f hemisphereUniform(Vector3f norm);
	virtual void getEmitPos(std::vector<InterPoint>& pos);
	virtual bool intersection(const Vector3f org, const Vector3f& ray_dir, InterPoint& inter_pt, bool is_out = true);
	bool intersectionTriangle(const Vector3f org, const Vector3f& ray_dir, const Vector3f vVert[], float& t, Vector2f& coordinate_uv, bool is_out = true);

private:
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	AABB aabb;
	BVH *bvh;
};

class Object_Sphere :public Object
{
public:
	Object_Sphere(const Vector3f& c_pos , const float& r);
	virtual ~Object_Sphere();
	Vector3f sphereUniform();
	Vector3f hemisphereUniform(Vector3f norm);
	virtual void getEmitPos(std::vector<InterPoint>& pos);
	virtual bool intersection(const Vector3f org, const Vector3f& ray_dir, InterPoint& inter_pt, bool is_out = true);

private:
	Vector3f center_pos;
	float radius;
};

#endif