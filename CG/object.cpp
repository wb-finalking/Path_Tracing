#include "object.h"
//***********************AABB*************************//
AABB::AABB() :max_x(0), min_x(0), max_y(0), min_y(0), max_z(0), min_z(0){}

AABB::~AABB(){}

void AABB::getPlane(Vector3f plane[], int index, int fixed_v)
{
	float boundary[3][2];
	boundary[0][0] = min_x;
	boundary[0][1] = max_x;
	boundary[1][0] = min_y;
	boundary[1][1] = max_y;
	boundary[2][0] = min_z;
	boundary[2][1] = max_z;

	switch (index)
	{
	case 0:
		plane[0] = Vector3f(boundary[0][fixed_v], boundary[1][0], boundary[2][0]);
		plane[1] = Vector3f(boundary[0][fixed_v], boundary[1][0], boundary[2][1]);
		plane[2] = Vector3f(boundary[0][fixed_v], boundary[1][1], boundary[2][0]);
		plane[3] = Vector3f(boundary[0][fixed_v], boundary[1][1], boundary[2][1]);
		break;
	case 1:
		plane[0] = Vector3f(boundary[0][0], boundary[1][fixed_v], boundary[2][0]);
		plane[1] = Vector3f(boundary[0][0], boundary[1][fixed_v], boundary[2][1]);
		plane[2] = Vector3f(boundary[0][1], boundary[1][fixed_v], boundary[2][0]);
		plane[3] = Vector3f(boundary[0][1], boundary[1][fixed_v], boundary[2][1]);
		break;
	case 2:
		plane[0] = Vector3f(boundary[0][0], boundary[1][0], boundary[2][fixed_v]);
		plane[1] = Vector3f(boundary[0][0], boundary[1][1], boundary[2][fixed_v]);
		plane[2] = Vector3f(boundary[0][1], boundary[1][0], boundary[2][fixed_v]);
		plane[3] = Vector3f(boundary[0][1], boundary[1][1], boundary[2][fixed_v]);
		break;
	default:
		break;
	}
}

bool AABB::intersection(const Vector3f org, const Vector3f& ray_dir)
{
	Vector3f plane[4];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
		{
			getPlane(plane,i,j);
			float t = 0;
			if (intersectionRectangle(org, ray_dir, plane, t))
				return true;
		}

	return false;
}

bool AABB::intersectionRectangle(const Vector3f org, const Vector3f& ray_dir, const Vector3f vVert[], float& t)
{
	Vector3f E1 = vVert[1] - vVert[0];
	Vector3f E2 = vVert[2] - vVert[0];
	Vector3f D = ray_dir;
	Vector3f T = org - vVert[0];

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

	if (std::fabs(det_M - 0) < 0.001)
		return false;

	t = det_M1 / det_M;
	float u = det_M2 / det_M;
	float v = det_M3 / det_M;
	if (u >= 0 && v >= 0 && u <= 1.0f&& v <= 1.0f&&t > 0)
		return true;
	return false;
}

//***********************Inter_Point*************************//
InterPoint::InterPoint() :material(NULL), is_empty(true), is_out(true), emit_dir(Vector3f(0,1,0)),
normal(Vector3f(0, 1, 0)), point(Vector3f(0, 0, 0)), t(0), P(-1)
{
	
}

InterPoint::~InterPoint(){}

//***********************Object*************************//
Object::Object() : material(NULL){}

Object::~Object()
{
	if (material != NULL)
		delete material;
}

void Object::setMaterial(Material* m)
{
	if (material != NULL)
	{
		delete material;
		material = m;
	}
	else
		material = m;
}

//***********************Object_Mesh*************************//
Object_Mesh::Object_Mesh(std::string &filename)
{
	if (!ReadObj::readfile(filename))
	{
		std::cout << "Reading obj fail..." << std::endl;
	}
	vertices.clear();
	faces.clear();
	vertices = std::move(ReadObj::getVertices());
	faces = std::move(ReadObj::getFaces());

	aabb.max_x = ReadObj::max_x;
	aabb.min_x = ReadObj::min_x;
	aabb.max_y= ReadObj::max_y;
	aabb.min_y = ReadObj::min_y;
	aabb.max_z = ReadObj::max_z;
	aabb.min_z = ReadObj::min_z;

	bvh =new BVH(&faces, &vertices, 10);
	bvh->root = new BVHNode();
	bvh->build(bvh->root, 0, faces.size());
}

Object_Mesh::~Object_Mesh()
{
	vertices.clear();
	faces.clear();
	delete bvh;
}

Vector3f Object_Mesh::hemisphereUniform(Vector3f norm)
{
	float r1 = rand() % 10000;
	float r2 = rand() % 10000;
	r1 = r1 / 10000;
	r2 = r2 / 10000;

	float theta = acos(1 - 2 * r1) / 2;
	float phi = 2 * PI*r2;

	Vector3f dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

	Vector3f Nx;
	if (std::fabs(norm(0))>std::fabs(norm(1)))
		Nx = Vector3f(norm(2), 0, -norm(0)).normalized();
	else
		Nx = Vector3f(0, -norm(2), norm(1)).normalized();
	Vector3f Ny = norm.cross(Nx).normalized();
	Vector3f Nz = norm;

	Vector3f new_dir = dir(0)*Nx + dir(1)*Ny + dir(2)*Nz;

	return new_dir.normalized();

}

void Object_Mesh::getEmitPos(std::vector<InterPoint>& pos)
{
	for (int face_id = 0; face_id < faces.size(); face_id++)
	{
		float u = rand() % 10000;
		float v = rand() % 10000;
		u = u / 10000;
		v = v / 10000;
		v *= (1 - u);

		Vector3f v0 = vertices[faces[face_id].verts[0]].coordinate;
		Vector3f v1 = vertices[faces[face_id].verts[1]].coordinate;
		Vector3f v2 = vertices[faces[face_id].verts[2]].coordinate;

		Vector3f pos_coordinate = u*(v1 - v0) + v*(v2 - v0);
		InterPoint tmp;
		tmp.point = pos_coordinate;
		tmp.normal = faces[face_id].norm;
		tmp.emit_dir = hemisphereUniform(tmp.normal);
		tmp.is_empty = false;
		tmp.material = material;
		pos.push_back(tmp);
	}
}

bool Object_Mesh::intersection(const Vector3f org, const Vector3f& ray_dir, InterPoint& inter_pt, bool is_out )
{
	float t;
	Vector3f normal;
	if (bvh->intersecting(org, ray_dir, t, normal))
	{
		inter_pt.point = org + t*ray_dir;
		inter_pt.normal = normal;
		inter_pt.t = t;
		inter_pt.is_out = is_out;
		inter_pt.material = this->material;
		return true;
	}
	else
		return false;

	//if (!aabb.intersection(org, ray_dir))
	//	return false;
	//for (Face f : faces)
	//{
	//	Vector3f verts[3];
	//	for (int i = 0; i < 3; i++)
	//	{
	//		verts[i] = vertices[f.verts[i]].coordinate;
	//	}
	//	float t;
	//	Vector2f coordinate_uv;
	//	if (intersectionTriangle(org, ray_dir, verts, t, coordinate_uv,is_out))
	//	{
	//		inter_pt.point = org + t*ray_dir;
	//		inter_pt.normal = f.norm;
	//		inter_pt.t = t;
	//		inter_pt.is_out = is_out;
	//		inter_pt.material = this->material;
	//		return true;
	//	}
	//}
	//return false;
}

bool Object_Mesh::intersectionTriangle(const Vector3f org, const Vector3f& ray_dir, const Vector3f vVert[], float& t, Vector2f& coordinate_uv , bool is_out)
{
	Vector3f E1 = vVert[1] - vVert[0];
	Vector3f E2 = vVert[2] - vVert[0];
	Vector3f D = ray_dir;
	Vector3f T = org - vVert[0];
	
	Matrix3f M,M1,M2,M3;
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

	if (std::fabs(det_M - 0) < 0.001)
		return false;

	t = det_M1 / det_M;

	float u = det_M2 / det_M;
	float v = det_M3 / det_M;
	coordinate_uv = Vector2f(u,v);
	if (u >= 0 && v >= 0 && u + v <= 1.0f&&t > 0)
	{
		Vector3f norm = E1.cross(E2).normalized();
		if (is_out&&norm.dot(-ray_dir) > 0)
			return true;
		else if (!is_out&&norm.dot(-ray_dir) < 0)
			return true;
	}
	return false;
}

//***********************Object_Sphere*************************//

Object_Sphere::Object_Sphere(const Vector3f& c_pos, const float& r)
{
	center_pos = c_pos;
	radius = r;
}

Object_Sphere::~Object_Sphere(){}

Vector3f Object_Sphere::sphereUniform()
{
	float r1 = rand() % 10000;
	float r2 = rand() % 10000;
	r1 = r1 / 10000;
	r2 = r2 / 10000;

	float theta = acos(1 - 2 * r1);
	float phi = 2 * PI*r2;

	Vector3f dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
	return dir.normalized();
}

Vector3f Object_Sphere::hemisphereUniform(Vector3f norm)
{
	float r1 = rand() % 10000;
	float r2 = rand() % 10000;
	r1 = r1 / 10000;
	r2 = r2 / 10000;

	float theta = acos(1 - 2 * r1) / 2;
	float phi = 2 * PI*r2;

	Vector3f dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

	Vector3f Nx;
	if (std::fabs(norm(0))>std::fabs(norm(1)))
		Nx = Vector3f(norm(2), 0, -norm(0)).normalized();
	else
		Nx = Vector3f(0, -norm(2), norm(1)).normalized();
	Vector3f Ny = norm.cross(Nx).normalized();
	Vector3f Nz = norm;

	Vector3f new_dir = dir(0)*Nx + dir(1)*Ny + dir(2)*Nz;

	return new_dir.normalized();

}

void Object_Sphere::getEmitPos(std::vector<InterPoint>& pos)
{
	Vector3f dir = sphereUniform();
	Vector3f pos_coordinate=center_pos+radius*dir;

	InterPoint tmp;
	tmp.point = pos_coordinate;
	tmp.normal = dir;
	tmp.emit_dir = hemisphereUniform(dir);
	tmp.is_empty = false;
	tmp.material = material;
	pos.push_back(tmp);
}

bool Object_Sphere::intersection(const Vector3f org, const Vector3f& ray_dir, InterPoint& inter_pt, bool is_out )
{
	Vector3f dir = ray_dir;
	Vector3f org_pos = org;

	float A = dir.dot(dir);
	float B = 2 * dir.dot(org_pos - center_pos);
	float C = (org_pos - center_pos).dot(org_pos - center_pos)-radius*radius;

	float delta = B*B - 4 * A*C;
	if (delta < 0)
		return false;

	float t1 = (-B - std::sqrt(delta)) / (2 * A);
	float t2 = (-B + std::sqrt(delta)) / (2 * A);

	if (t1 > 0)
		inter_pt.t = t1;
	else if (t2 > 0)
		inter_pt.t = t2;
	else
		return false;

	inter_pt.point = org_pos + inter_pt.t*dir;
	inter_pt.is_out = is_out;
	if (is_out && (-ray_dir).dot((inter_pt.point - center_pos).normalized()) >= 0)
	{
		inter_pt.normal = (inter_pt.point - center_pos).normalized();
		inter_pt.material = this->material;
		return true;
	}
	else if (!is_out && (-ray_dir).dot((inter_pt.point - center_pos).normalized()) <= 0)
	{
		inter_pt.normal = (inter_pt.point - center_pos).normalized();
		inter_pt.material = this->material;
		return true;
	}

	return false;

}