#include "Material.h"

//******************Material*******************//
Material::Material(MaterialType m) :material_type(m)
{
}

Material::~Material()
{
}


//******************Light*******************//
Light::Light(float density) :Material(LIGHT), light_density(density)
{
}

Light::~Light()
{
}

void Light::setLightDensity(float density)
{
	light_density = density;
}

float Light::fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out)
{
	return 1 / (2 * PI);
}

Vector3f Light::fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P)
{
	float cos = std::fabs(normal.dot(dir_in));
	return cos*Vector3f(light_density, light_density, light_density);
}

Vector3f Light::radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac , bool is_out )
{
	return Vector3f(light_density, light_density, light_density);
}

//******************Diffuse*******************//
Diffuse::Diffuse(Vector3f c) :Material(DIFFUSE), color(c)
{
}

Diffuse::~Diffuse()
{
}

Vector3f Diffuse::hemisphereUniform(Vector3f norm)
{
	float r1 = rand() % 10000;
	float r2 = rand() % 10000;
	r1 = r1 / 10000;
	r2 = r2 / 10000;

	float theta = acos(1 - 2 * r1) / 2;
	float phi = 2 * PI*r2;

	Vector3f dir(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));

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

float Diffuse::fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out)
{
	return 1 / (2 * PI);
}

Vector3f Diffuse::fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P)
{
	Vector3f color_tmp = L;
	color_tmp[0] = color_tmp[0] * color[0];
	color_tmp[1] = color_tmp[1] * color[1];
	color_tmp[2] = color_tmp[2] * color[2];

	float factor = dir_out.dot(normal);
	//color_tmp *= factor / (2 * PI);
	if (P != -1)
	{
		float p_tmp = P / 2 + 1 / (4 * PI);
		factor *= 1/(2*PI*p_tmp);
	}
	//color_tmp *= factor;
	color_tmp *= std::fabs(dir_out.dot(normal)) / (2 * PI);
	return color_tmp;
}

Vector3f Diffuse::radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac, bool is_out)
{
	Vector3f color_tmp = color;
	Vector3f color_radiated;

	Vector3f dir_out = hemisphereUniform(inter_p.normal);
	Vector3f L = scene->MCSDRayTracer(inter_p.point, dir_out, depth - 1, path, refrac, is_out);
	color_radiated[0] = color_tmp[0] * L[0];
	color_radiated[1] = color_tmp[1] * L[1];
	color_radiated[2] = color_tmp[2] * L[2];

	//if (color_radiated[0]>0 || color_radiated[1]>0 || color_radiated[2]>0)
	//	std::cout << "diffuse" << color_radiated[0] << "    " << color_radiated[1] << "    " << color_radiated[2] << std::endl;

	float factor = dir_out.dot(inter_p.normal);
	//color_radiated *= factor/(2*PI);
	color_radiated *= factor;
	return color_radiated;
}

//******************Mirror*******************//
Mirror::Mirror() :Material(MIRROR)
{
}

Mirror::~Mirror()
{
}

Vector3f Mirror::generateDir(Vector3f norm, Vector3f dir_V)
{
	Vector3f dir_reflected = - 2* (dir_V.dot(norm))*norm + dir_V;

	return dir_reflected.normalized();
}

float Mirror::fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out)
{
	Vector3f dir_out_tmp = generateDir(normal, dir_in);
	float dot = dir_out_tmp.dot(dir_out);

	if (std::fabs(dot - 1) < 0.001)
	{
		return 1;
	}
	else
		return 0;
}

Vector3f Mirror::fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P)
{
	Vector3f dir_out_tmp = generateDir(normal, dir_in);
	float dot = dir_out_tmp.dot(dir_out);
	float factor = 1;
	if (P != -1)
	{
		float p_tmp = P / 2 + 0.5;
		factor *= 1/p_tmp;
	}

	if (std::fabs(dot - 1) < 0.001)
	{
		//return factor*L;
		return L;
	}
	else
		return Vector3f(0,0,0);
}

Vector3f Mirror::radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac, bool is_out)
{
	//Vector3f color;
	//color = (dir_L*norm)*L;
	Vector3f dir_out = generateDir(inter_p.normal, dir_in);
	Vector3f L = scene->MCSDRayTracer(inter_p.point, dir_out, depth - 1, path, refrac, is_out);

	return L;
}
//******************Glass*******************//
Glass::Glass() :Material(GLASS), refractive_index(1.46)
{
}

Glass::~Glass()
{
}

bool  Glass::generateRefractedDir(const Vector3f &normal, const Vector3f &dir_in, bool is_out, Vector3f& dir_out)
{
	float eta;
	Vector3f norm;
	if (is_out)
	{
		eta = refractive_index;
		norm = normal;
	}
	else
	{
		eta = 1 / refractive_index;
		norm = -normal;
	}

	//t1=(1/eta)(L+Ncos¦È1)
	//t2=-Ncos¦È2
	//T=t1+t2
	float cos1 = norm.dot(-dir_in);
	if (cos1*cos1 + eta*eta < 1)
		return false;
	float cos2 = std::sqrt(1 - (1 - cos1*cos1) / (eta*eta));

	dir_out = dir_in / eta + (cos1 / eta - cos2)*norm;
	return true;
}

bool  Glass::generateReflectedDir(const Vector3f &normal, const Vector3f &dir_in, bool is_out, Vector3f& dir_out)
{
	Vector3f norm;
	if (is_out)
	{
		norm = normal;
	}
	else
	{
		norm = -normal;
	}
	dir_out = -2 * (dir_in.dot(norm))*norm + dir_in;
	return true;
}

float Glass::fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out)
{
	Vector3f color(0, 0, 0);
	//color = (dir_L*norm)*L;
	Vector3f dir_out_refracted;
	Vector3f dir_out_reflected;

	bool is_refracted = generateRefractedDir(normal, dir_in, is_out, dir_out_refracted);
	generateReflectedDir(normal, dir_in, is_out, dir_out_reflected);

	Vector3f L_refracted, L_reflected;
	if (is_refracted)
	{
		float w1, w2;
		w1 = std::fabs(dir_in.dot(normal));
		w2 = std::fabs(dir_out.dot(normal));
		w1 = w1 / (w1 + w2);
		w2 = w1 / (w1 + w2);

		float dot_refracted = dir_out_refracted.dot(dir_out);
		float dot_reflected = dir_out_reflected.dot(dir_out);
		if (std::fabs(dot_refracted - 1) < 0.001)
		{
			return 1;
		}
		else if (std::fabs(dot_reflected - 1) < 0.001)
		{
			return 1;
		}
	}
	else
	{
		float dot_reflected = dir_out_reflected.dot(dir_out);

		if (std::fabs(dot_reflected - 1) < 0.001)
		{
			return 1;
		}
	}

	return 0;
}

Vector3f Glass::fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P)
{
	Vector3f color(0, 0, 0);
	//color = (dir_L*norm)*L;
	Vector3f dir_out_refracted;
	Vector3f dir_out_reflected;

	bool is_refracted = generateRefractedDir(normal, dir_in, is_out, dir_out_refracted);
	generateReflectedDir(normal, dir_in, is_out, dir_out_reflected);

	float factor = 1;
	if (P != -1)
	{
		float p_tmp = P / 2 + 0.5;
		factor *= 1/p_tmp;
	}

	Vector3f L_refracted, L_reflected;
	if (is_refracted)
	{
		float w1, w2;
		w1 = 1;
		w2 = (1 - std::fabs(dir_in.dot(normal)))/20;
		float w = w1 + w2;
		w1 /= w;
		w2 /= w;

		float dot_refracted = dir_out_refracted.dot(dir_out);
		float dot_reflected = dir_out_reflected.dot(dir_out);
		if (std::fabs(dot_refracted - 1) < 0.001)
		{
			//return w1*L;
			return L;
		}
		else if (std::fabs(dot_reflected - 1) < 0.001)
		{
			//return factor*w2*L;
			return L;
		}
	}
	else
	{
		float dot_reflected = dir_out_reflected.dot(dir_out);

		if (std::fabs(dot_reflected - 1) < 0.001)
		{
			//return factor*L;
			return L;
		}
	}

	return color;
}

Vector3f Glass::radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac, bool is_out)
{
	Vector3f color(0,0,0);
	//color = (dir_L*norm)*L;
	Vector3f dir_out_refracted;
	Vector3f dir_out_reflected;

	bool is_refracted=generateRefractedDir(inter_p.normal, dir_in, is_out, dir_out_refracted);
	generateReflectedDir(inter_p.normal, dir_in, is_out, dir_out_reflected);

	Vector3f L_refracted, L_reflected;
	if (is_refracted)
	{
		L_refracted = scene->MCSDRayTracer(inter_p.point, dir_out_refracted, depth - 1,path, refrac, !is_out);
		L_reflected = scene->MCSDRayTracer(inter_p.point, dir_out_reflected, depth - 1,path, false, is_out);
	}
	else
		L_reflected = scene->MCSDRayTracer(inter_p.point, dir_out_reflected, depth - 1, path, refrac, is_out);

	if (is_refracted)
	{
		float w1, w2;
		float r1, r2;
		float cosi = std::fabs(dir_in.dot(inter_p.normal));
		float cost = std::fabs(dir_out_refracted.dot(inter_p.normal));
		float eta = is_out ? refractive_index : 1.0 / refractive_index;
		r1 = (eta*cosi - cost) / (eta*cosi + cost);
		r2 = (cosi - eta*cost) / (cosi + eta*cost);
		w2 = (r1*r1+r2*r2)/2;
		w1 = 1 - w2;
		//w2 = (1-std::fabs(dir_in.dot(inter_p.normal)) )/20;
		/*float w = w1 + w2;
		w1 /= w;
		w2 /= w;*/

		color = w1*L_refracted + w2*L_reflected;
	}
	else
		color = L_reflected;


	return color;
}


//******************Metal*******************//
Metal::Metal(Vector3f c, float x, float y) :Material(METAL), rho(0.05), color(c), alpha_x(x), alpha_y(y)
{

}

Metal::~Metal()
{

}

Vector3f Metal::hemisphereWard(Vector3f norm, Vector3f dir_V)
{
	float r1 = rand() % 10000;
	float r2 = rand() % 10000;
	r1 = r1 / 10000;
	r2 = r2 / 10000;
	
	//alpha_x != alpha_y
	//while (r1 == 0.25 || r1 == 0.75)
	//{
	//	r1 = rand() % 10000;
	//	r1 = r1 / 10000;
	//}
	//float phi_h = std::atan(alpha_y*tan(2*PI*r1)/alpha_x);
	//if (r1 > 0.25&&r1 < 0.75)
	//{
	//	phi_h += PI;
	//}
	//while (r2 == 0)
	//{
	//	r2 = rand() % 10000;
	//	r2 = r2 / 10000;
	//}
	//float theta_h = atan(sqrt(-log10(r2) / (cos(phi_h)*cos(phi_h) / (alpha_x*alpha_x) + sin(phi_h)*sin(phi_h) / (alpha_y*alpha_y))));

	//alpha_x==alpha_y
	float phi_h = 2 * PI*r1;
	float theta_h = atan(sqrt(-log10(r2) *alpha_x*alpha_x ));

	//generate dir_out accoding to the h
	Vector3f dir(sin(theta_h)*cos(phi_h), sin(theta_h)*sin(phi_h), cos(theta_h));

	Vector3f Nx;
	if (std::fabs(norm(0))>std::fabs(norm(1)))
		Nx = Vector3f(norm(2), 0, -norm(0)).normalized();
	else
		Nx = Vector3f(0, -norm(2), norm(1)).normalized();
	Vector3f Ny = norm.cross(Nx).normalized();
	Vector3f Nz = norm;

	Vector3f new_dir = dir(0)*Nx + dir(1)*Ny + dir(2)*Nz;
	new_dir.normalize();

	Vector3f dir_reflected = -2 * (dir_V.dot(new_dir))*new_dir + dir_V;

	return dir_reflected.normalized();
}

float Metal::fixedProbability(const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out)
{
	float cos_theta_h = normal.dot((-dir_in + dir_out).normalized());
	float sin_theta_h = sqrt(1 - cos_theta_h*cos_theta_h);
	float tan_theta_h = sin_theta_h / cos_theta_h;

	float cos_theta_in = normal.dot(-dir_in);
	float cos_theta_out = normal.dot(dir_out);

	if (fabs(cos_theta_h) < 0.001)
		return 0;
	float brdf = pow(2.73, -tan_theta_h*tan_theta_h / (alpha_x*alpha_x)) / (4 * PI*alpha_x*alpha_x*sqrt(cos_theta_in*cos_theta_out));

	return brdf;
}

Vector3f Metal::fixedRadiate(const Vector3f& L, const Vector3f& normal, const Vector3f& dir_in, const Vector3f& dir_out, bool is_out, float P)
{
	float cos_theta_h = normal.dot((-dir_in + dir_out).normalized());
	float sin_theta_h = sqrt(1 - cos_theta_h*cos_theta_h);
	float tan_theta_h = sin_theta_h / cos_theta_h;

	float cos_theta_in = std::fabs(normal.dot(-dir_in));
	float cos_theta_out = std::fabs(normal.dot(dir_out));

	if (fabs(cos_theta_h) < 0.001)
		return Vector3f(0,0,0);
	float brdf = pow(2.73, -tan_theta_h*tan_theta_h*alpha_x*alpha_x) / (4 * PI*alpha_x*alpha_x*sqrt(cos_theta_in*cos_theta_out));
	
	float factor = dir_out.dot(normal);
	if (P != -1)
	{
		float p_tmp = P / 2 + brdf/2;
		factor *= brdf/p_tmp;
	}

	//return rho*factor*L;
	return 0.01*rho*brdf*std::fabs(dir_out.dot(normal))*L;
}

Vector3f Metal::radiate(InterPoint inter_p, const Vector3f& dir_in, int depth, Scene *scene, std::vector<InterPoint>& path, bool refrac, bool is_out)
{
	Vector3f color_tmp = color;
	Vector3f color_radiated;

	Vector3f dir_out = hemisphereWard(inter_p.normal, dir_in);
	if (dir_out.dot(inter_p.normal) < 0)
	{
		//std::cout << "out  :" << dir_out.dot(inter_p.normal) << std::endl;
		return Vector3f(0, 0, 0);
	}

	float brdf = fixedProbability(inter_p.normal, dir_in, dir_out, is_out);

	Vector3f L = scene->MCSDRayTracer(inter_p.point, dir_out, depth - 1, path, refrac, is_out);
	color_radiated[0] = color_tmp[0] * L[0];
	color_radiated[1] = color_tmp[1] * L[1];
	color_radiated[2] = color_tmp[2] * L[2];

	float factor = rho*dir_out.dot(inter_p.normal);
	color_radiated *= factor;
	return color_radiated;
}