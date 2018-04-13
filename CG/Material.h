#ifndef _MATERIAL_
#define _MATERIAL_
#include <eigen/dense>
#include "Scene.h"
using namespace Eigen;

enum MaterialType{ LIGHT, DIFFUSE, MIRROR, GLASS, METAL };
#define PI 3.1415926

class InterPoint;
class Scene;
class Material
{
public:
	Material(MaterialType m);
	virtual ~Material()=0;

	//probability
	virtual float fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out) = 0;

	//computing color according to fixed transfer path
	virtual Vector3f fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P=-1) = 0;

	virtual Vector3f radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac=false, bool is_out = true) = 0;

	MaterialType material_type;

private:
};

class Light :public Material
{
public:
	Light(float density);
	~Light();

	void setLightDensity(float density);
	virtual float fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out);
	virtual Vector3f fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P = -1);
	Vector3f radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac = false, bool is_out = true);
private:
	float light_density;
};

class Diffuse :public Material
{
public:
	Diffuse(Vector3f c);
	~Diffuse();

	Vector3f hemisphereUniform(Vector3f norm);
	virtual float fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out);
	virtual Vector3f fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P = -1);
	Vector3f radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac = false, bool is_out = true);

private:
	Vector3f color;
};

class Mirror :public Material
{
public:
	Mirror();
	~Mirror();

	Vector3f generateDir(Vector3f norm , Vector3f dir_V);
	virtual float fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out);
	virtual Vector3f fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P = -1);
	Vector3f radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac = false, bool is_out = true);

private:

};

class Glass :public Material
{
public:
	Glass();
	~Glass();
	virtual float fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out);
	virtual Vector3f fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P = -1);
	bool  generateRefractedDir(const Vector3f &normal, const Vector3f &dir_in, bool is_out, Vector3f& dir_out);
	bool  generateReflectedDir(const Vector3f &normal, const Vector3f &dir_in, bool is_out, Vector3f& dir_out);
	Vector3f radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac = false, bool is_out = true);
private:
	float refractive_index;
};

class Metal :public Material
{
public:
	Metal(Vector3f c, float x,float y);
	~Metal();

	virtual float fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out);
	Vector3f hemisphereWard(Vector3f norm, Vector3f dir_V);
	virtual Vector3f fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P = -1);
	virtual Vector3f radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac = false, bool is_out = true);

private:
	float rho;
	float alpha_x;
	float alpha_y;
	Vector3f color;
};



#endif