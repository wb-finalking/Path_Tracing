#include <Scene.h>

std::atomic_int Scene::computed_row{0};

Scene::Scene() : viewport_width(8), viewport_height(5), max_depth(14), 
sample_num(64), light_intensity(35), near_plane(10),cam_pos(Vector3f(0, 4, 30)),
cam_view(Vector3f(0, 0, -1)), cam_up(Vector3f(0, 1, 0)), cam_right(Vector3f(1, 0, 0)),
environment_color(Vector3f(0, 0, 0)), img_width(801), img_height(531), thread_num(2)
{
	buffer = new Vector3f[img_height*img_width];
}

Scene::~Scene()
{
	for (Object* obj : m_objects)
		delete obj;
	m_objects.clear();

	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
	}
}

void Scene::render(RayTracingType raytracing_type)
{	
	computed_row = 0;
	multithread(raytracing_type);
	for (int i = 0; i < (int)m_thread_list.size(); i++)
		m_thread_list[i].join();
}

void Scene::multithread(RayTracingType raytracing_type)
{
	m_thread_list.clear();
	int stride = img_height / thread_num;
	int start_row = 0, end_row = 0;

	for (int i = 0; i < thread_num; i++)
	{
		end_row = ((i == thread_num - 1) ? img_height : start_row + stride);
		m_thread_list.push_back(std::thread(&Scene::subRender, this, start_row, end_row, raytracing_type));
		start_row = end_row;
	}

	m_thread_list.push_back(std::thread(&Scene::emitStatus, this));
}

void Scene::subRender(int start_row, int end_row, RayTracingType raytracing_type)
{
	Vector3f view_center_pos = cam_pos + near_plane*cam_view;
	Vector3f start_pos = view_center_pos - (1.0*viewport_width / 2.0)*cam_right + (1.0*viewport_height / 2.0)*cam_up;

	Vector3f color(0, 0, 0);
	for (int row = start_row; row < end_row; row++)
	{
		for (int col = 0; col < img_width; col++)
		{
			float r = 1.0*row*viewport_height / img_height;
			float c = 1.0*col*viewport_width / img_width;
			Vector3f org = start_pos + c*cam_right - r*cam_up;
			Vector3f dir = (org - cam_pos).normalized();
			color = Vector3f(0, 0, 0);

			if (raytracing_type == Ordinary)
			{
				//ordinay raytracing
				color = RayTracer(org, dir);
				color *= 255;
				//std::cout <<"color : "<< color << std::endl;
			}				
			else if (raytracing_type == MCSD)
			{
				//MCSD raytracing
				int valid_num = 0;
				for (int sample_index = 0; sample_index < sample_num; sample_index++)
				{
					float x_noise = noise(viewport_width, img_width);
					float y_noise = noise(viewport_height, img_height);
					Vector3f org_noise = org + x_noise*cam_right - y_noise*cam_up;
					Vector3f dir_noise = (org_noise - cam_pos).normalized();
					Vector3f color_tmp(0, 0, 0);
					std::vector<InterPoint> path;
					path.reserve(max_depth+2);
					//color_tmp = MCSDRayTracer(org_noise, dir_noise, max_depth);
					//if (color_tmp[0] > 0 || color_tmp[1] > 0 || color_tmp[2] > 0)
					//{
					//	color += color_tmp;
					//	valid_num++;
					//}
					color += MCSDRayTracer(org_noise, dir_noise, max_depth, path);
				}
				//if (valid_num > 0)
				//	color /= valid_num;
				color /= sample_num;
				clip(color);

				for (int i = 0; i < 3; i++)
				{
					int c = int(pow(color[i], 0.4545f)*255.0f + 0.5f);
					//int c = int(pColor[i] + 0.5f);
					c = (c > 255) ? 255 : c;
					color[i] = c;
				}
			}
			else if (raytracing_type == MCBD)
			{
				//MCBD raytracing	
				int valid_num = 0;
				for (int sample_index = 0; sample_index < sample_num; sample_index++)
				{
					float x_noise = noise(viewport_width, img_width);
					float y_noise = noise(viewport_height, img_height);
					Vector3f org_noise = org + x_noise*cam_right - y_noise*cam_up;
					Vector3f dir_noise = (org_noise - cam_pos).normalized();
					Vector3f color_tmp(0, 0, 0);
					color_tmp = MCBDRayTracer(org_noise, dir_noise, max_depth);
					if (color_tmp[0] >= 0 && color_tmp[1] >= 0 && color_tmp[2] >= 0)
					{
						clip2(color_tmp);
						color += color_tmp;
						valid_num++;
					}
					//color += MCBDRayTracer(org_noise, dir_noise, max_depth);
				}
				if (valid_num > 0)
					color /= valid_num;
				//color /= sample_num;
				clip(color);

				for (int i = 0; i < 3; i++)
				{
					int c = int(pow(color[i], 0.4545f)*255.0f + 0.5f);
					//int c = int(color[i]*255 + 0.5f);
					c = (c > 255) ? 255 : c;
					color[i] = c;
				}
			}
			buffer[row*img_width + col] = color;
			//if (color[0]>0||color[1]>0||color[2]>0)
			//std::cout << color[0] << "    " << color[1] << "    " << color[2] << std::endl;
			//std::cout << "col : " << col << "   row : " << row << std::endl;
		}
		std::cout << "row : " << row << std::endl;
		computed_row++;
		emit statusChange(int(computed_row));
		//std::cout << "computed_row : " << computed_row << std::endl;
	}
}

void Scene::loadScene(std::vector<std::string>&obj_files)
{
	for (Object* obj : m_objects)
		delete obj;
	m_objects.clear();

	for (std::string filename : obj_files)
	{
		m_objects.push_back(new Object_Mesh(filename));
	}
}

void Scene::loadObj(std::string filename)
{
	m_objects.push_back(new Object_Mesh(filename));
}

void Scene::loadSphere(const Vector3f center, const float r)
{
	m_objects.push_back(new Object_Sphere(center, r));
}

void Scene::appendMaterial(Material* m)
{
	int obj_id = m_objects.size()-1;

	if (m_objects[obj_id]->material == NULL)
		m_objects[obj_id]->material = m;
	else
		delete m;
}

void Scene::clearObjs()
{
	for (Object* obj : m_objects)
		delete obj;
	m_objects.clear();
}

Vector3f Scene::RayTracer(const Vector3f& org, const Vector3f& dir)
{
	float min_t;
	bool intersected=false;
	InterPoint tmp_inter_p;
	InterPoint inter_p;
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->intersection(org, dir, tmp_inter_p))
		{
			//std::cout <<" tmp_inter_pt "<< tmp_inter_p.t << std::endl;
			if (!intersected || min_t > tmp_inter_p.t)
			{
				intersected = true;
				min_t = tmp_inter_p.t;
				inter_p = tmp_inter_p;
			}
		}
	}

	if (intersected)
		//return Vector3f(0.25,0.25,0.75);
		return inter_p.normal;
	else
		return environment_color;
}

bool Scene::sceneIntersecting(const Vector3f& org, const Vector3f& dir, InterPoint& inter_p, bool is_out)
{
	float min_t;
	bool intersected = false;
	InterPoint tmp_inter_p;
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->intersection(org, dir, tmp_inter_p, is_out))
		{
			if (!intersected || min_t > tmp_inter_p.t)
			{
				intersected = true;
				min_t = tmp_inter_p.t;
				inter_p = tmp_inter_p;
			}
		}
	}

	if (intersected)
		return true;
	else
		return false;
}

Vector3f Scene::MCSDRayTracer(const Vector3f& org, const Vector3f& dir, int depth, std::vector<InterPoint>& path,bool refrac, bool is_out)
{
	if (depth < 0)
		return environment_color;

	InterPoint inter_p;
	Vector3f color(0,0,0);
	if (!sceneIntersecting(org, dir, inter_p,is_out))
		return environment_color;
	
	inter_p.is_empty = false;
	if (refrac)
		path[max_depth - depth+1] = inter_p;
	//*************self iteration**************//
	//generating dir according material
	//dir=...
	//color_tmp=MCSDRayTracer(inter_p.point , dir , depth-1);

	//computing color according to material 
	//color=.../p(dir)

	//*************radiate iteration**************//
	if (inter_p.material!=NULL)
		color = inter_p.material->radiate(inter_p, dir, depth, this,path, refrac, is_out);

	return color;
}

Vector3f Scene::pathLinker(std::vector<InterPoint>&view_path, std::vector<InterPoint>&light_path)
{
	Vector3f color(0, 0, 0);
	int view_point = 0;//contain lightsource
	for (int i = 0; i < max_depth + 2; i++)
	{
		if (!view_path[i].is_empty)
			view_point++;
		else
			break;
	}

	int light_point = 0;//contain viewpoint
	for (int i = 0; i < max_depth + 2; i++)
	{
		if (!light_path[i].is_empty)
			light_point++;
		else
			break;
	}

	//modify is_out but will also modify when checking
	for (int i = 0; i < light_point - 1; i++)
	{
		if (!(light_path[i].is_out&&light_path[i + 1].is_out))
			light_path[i].is_out = !light_path[i].is_out;
	}

	int valid_num = 0;
	int check_num = 0;
	float total_p = 0;
	for (int i = 1; i < view_point; i++)
	{
		for (int j = 0; j < light_point; j++)
		{
			Vector3f color_tmp(0, 0, 0);
			float path_p;
			if (pathCheck(view_path, i, light_path, j, color_tmp, path_p))
			{
				color += path_p*path_p*color_tmp;
				valid_num++;
				total_p += path_p*path_p;
			}
			//color += color_tmp;
			check_num++;
		}
	}
	if (valid_num>0)
		color /= total_p;
		//color /= valid_num;
	//color /= check_num;


	//float num = 0;
	//for (int k = 0; k < view_point + light_point-1; k++)
	//{
	//	int valid_num = 0;
	//	float total_p = 0;
	//	Vector3f color_sum(0,0,0);
	//	for (int i = 0; i <= k; i++)
	//	{
	//		int j = k - i;
	//		if (i < (view_point - 1) && j < light_point)
	//		{
	//			Vector3f color_tmp(0, 0, 0);
	//			float path_p;
	//			if (pathCheck(view_path, i+1, light_path, j, color_tmp, path_p))
	//			{
	//				color_sum += path_p*path_p*color_tmp;
	//				valid_num++;
	//				total_p += path_p*path_p;
	//			}
	//		}
	//	}
	//	if (valid_num > 0)
	//	{
	//		color_sum /= total_p;
	//		color += color_sum;
	//		num++;
	//	}
	//}
	//color /= num;

	return color;
}

bool Scene::pathCheck(std::vector<InterPoint>&view_path, int indexLinked_view, std::vector<InterPoint>&light_path, int indexLinked_light, Vector3f &color, float &path_p)
{
	//******************checking block*********************//
	bool blocked = true;
	bool is_out_linked = true;//need modifing
	bool intersected;
	Vector3f org = view_path[indexLinked_view].point;
	Vector3f dir = (light_path[indexLinked_light].point - view_path[indexLinked_view].point).normalized();
	InterPoint inter_p;
	intersected=sceneIntersecting( org, dir, inter_p, true);
	if (intersected)
	{
		Vector3f v1 = (light_path[indexLinked_light].point - view_path[indexLinked_view].point);
		Vector3f v2 = (inter_p.point - view_path[indexLinked_view].point);
		float t1 = v1.dot(v1);
		float t2 = v2.dot(v2);
		if (std::abs(t1 - t2) < 0.001)
		{
			blocked = false;
			is_out_linked = true;
		}
	}
	if (blocked && sceneIntersecting(org, dir, inter_p, false))
	{
		Vector3f v1 = (light_path[indexLinked_light].point - view_path[indexLinked_view].point);
		Vector3f v2 = (inter_p.point - view_path[indexLinked_view].point);
		float t1 = v1.dot(v1);
		float t2 = v2.dot(v2);
		if (std::abs(t1 - t2) < 0.001)
		{
			blocked = false;
			is_out_linked = false;
		}
	}
	if (blocked)
		return false;

	//******************computing probability at the linked node*********************//
	float P1 = -1;
	float P2 = -1;
	if (indexLinked_light <= 0)
	{
		P1 = 1 / (2 * PI);

		Vector3f dir_in = (view_path[indexLinked_view].point - view_path[indexLinked_view - 1].point).normalized();
		Vector3f dir_out = (light_path[indexLinked_light].point - view_path[indexLinked_view].point).normalized();
		P2 = view_path[indexLinked_view].material->fixedProbability(view_path[indexLinked_view].normal, dir_in, dir_out, view_path[indexLinked_view].is_out);
	}
	else
	{
		//Vector3f dir_in = (view_path[indexLinked_view].point - path_node[end_id].point).normalized();
		Vector3f dir_out = (view_path[indexLinked_view].point - light_path[indexLinked_light].point).normalized();
		Vector3f dir_in = (light_path[indexLinked_light].point - light_path[indexLinked_light - 1].point).normalized();
		P1 = light_path[indexLinked_light].material->fixedProbability(light_path[indexLinked_light].normal, dir_in, dir_out, light_path[indexLinked_light].is_out);

		dir_in = (view_path[indexLinked_view].point - view_path[indexLinked_view - 1].point).normalized();
		dir_out = (light_path[indexLinked_light].point - view_path[indexLinked_view].point).normalized();
		P2 = view_path[indexLinked_view].material->fixedProbability(view_path[indexLinked_view].normal, dir_in, dir_out, view_path[indexLinked_view].is_out);
	}
	if (std::fabs(P1) < 0.001&&std::fabs(P2) < 0.001)
		return false;


	//******************path computing*********************//
	std::vector<InterPoint> path_node;
	//path_node.push_back(light_path[0]);
	for (int i = 0; i < indexLinked_light+1; i++)
	{
		path_node.push_back(light_path[i]);
	}	
	for (int i = indexLinked_view; i >=0; i--)
	{
		path_node.push_back(view_path[i]);
	}
	path_node[indexLinked_light + 1].P = P1;//assign probability at the linked node
	//path_node.push_back(view_path[0]);

	//******************modify is_out*********************//
	path_node[indexLinked_light].is_out = is_out_linked;

	//first node is light source
	//final node is viewpoint
	Vector3f color_tmp(0,0,0);
	Vector3f normal(0, 1, 0);
	Vector3f dir_in(0,1,0);
	Vector3f dir_out(0,1,0);
	bool is_out = true;
	for (int i=0; i < path_node.size()-1; i++)
	{
		normal = path_node[i].normal;
		is_out = path_node[i].is_out;
		dir_in = (path_node[i].point - path_node[i+1].point).normalized();
		color_tmp = path_node[i].material->fixedRadiate(color_tmp, normal, dir_in, dir_out, is_out, path_node[i].P);
		dir_out = (path_node[i].point - path_node[i + 1].point).normalized();
	}
	color = color_tmp;

	//******************MC path computing*********************//
	path_p = 1;
	if (indexLinked_view > 1)
	{
		for (int i = 1; i <indexLinked_view - 1; i++)
		{
			dir_in = (view_path[i].point - view_path[i - 1].point).normalized();
			dir_out = (view_path[i + 1].point - view_path[i].point).normalized();
			float p = view_path[i].material->fixedProbability(view_path[i].normal, dir_in, dir_out, view_path[i].is_out);
			color /= p;
			path_p *= p;
		}
	}
	//color /= (P1 + P2) / 2;
	//path_p *= (P1 + P2) / 2;
	if (indexLinked_light > 1)
	{
		for (int i = 1; i < indexLinked_light - 1; i++)
		{
			dir_in = (light_path[i].point - light_path[i - 1].point).normalized();
			dir_out = (light_path[i + 1].point - light_path[i].point).normalized();
			float p = light_path[i].material->fixedProbability(light_path[i].normal, dir_in, dir_out, light_path[i].is_out);
			color /= p;
			path_p *= p;
		}
		color /= 1 / (2 * PI);
		path_p *= 1 / (2 * PI);
		//color /= (P1 + P2) / 2;
		//path_p *= (P1 + P2) / 2;
	}
	else if (indexLinked_light == 1)
	{
		color /= 1 / (2 * PI);
		path_p *= 1 / (2 * PI);
		//color /= (P1 + P2) / 2;
		//path_p *= (P1 + P2) / 2;
	}
		

	return true;
}

Vector3f Scene::MCBDRayTracer(const Vector3f& org, const Vector3f& dir, int depth, bool is_out)
{
	Vector3f color(0,0,0);
	std::vector<InterPoint> view_path;
	view_path.resize(max_depth + 2);
	InterPoint view_point;
	view_point.is_empty = false;
	view_point.emit_dir = dir;
	view_point.point = org;
	view_path[0] = view_point;

	color = MCSDRayTracer(org, dir, depth, view_path, is_out );

	//if single direction MCRayTracer get the lightsource
	if (color[0] > 0 || color[1] > 0 || color[2] > 0)
		return color;

	if (view_path[1].is_empty)
		return color;
	// emit the path from lightsource
	std::vector<InterPoint> emit_pos;
	for (int obj_id = 0; obj_id < m_objects.size(); obj_id++)
	{
		if (m_objects[obj_id]->material->material_type == LIGHT)
		{
			m_objects[obj_id]->getEmitPos(emit_pos);
		}
	}

	//int source_num = emit_pos.size();
	//int valid_num=0;
	//color = Vector3f(0,0,0);
	//for (int source_id = 0; source_id < source_num; source_id++)
	//{
	//	std::vector<InterPoint> light_path;
	//	light_path.resize(max_depth + 2);
	//	light_path[0] = emit_pos[source_id];
	//	MCSDRayTracer(emit_pos[source_id].point, emit_pos[source_id].emit_dir, depth, light_path, is_out );

	//	//Vector3f color_tmp = pathLinker(view_path,light_path);
	//	//if (color_tmp[0] > 0 || color_tmp[1]>0 || color_tmp[2] > 0)
	//	//{
	//	//	color += color_tmp;
	//	//	valid_num++;
	//	//}
	//	color += pathLinker(view_path, light_path);
	//}
	////if (valid_num>0)
	////	color /= valid_num;
	//color /= source_num;

	int source_num = emit_pos.size();
	int valid_num=0;
	color = Vector3f(0,0,0);
	int source_id = rand() % source_num;
	std::vector<InterPoint> light_path;
	light_path.resize(max_depth + 2);
	light_path[0] = emit_pos[source_id];
	MCSDRayTracer(emit_pos[source_id].point, emit_pos[source_id].emit_dir, depth, light_path, is_out );
	color = pathLinker(view_path, light_path);


	return color;
}

void Scene::renderViewport(QImage& img_scene)
{
	for (int i = 0; i<img_height; i++)
	{
		int row_ind = i*img_width;
		for (int j = 0; j<img_width; j++)
		{
			int index = row_ind + j;
			uint color = 0;
			Vector3f tmp_color = buffer[index];
			//img_scene.setPixel(j, i, qRgb((int)tmp_color[0], (int)tmp_color[1], (int)tmp_color[2]));
			img_scene.setPixel(j, i, qRgb((int)std::fabs(tmp_color[0]), (int)std::fabs(tmp_color[1]), (int)std::fabs(tmp_color[2])));
		}
	}
}

void Scene::clip(Vector3f& color)
{
	for (int i = 0; i<3; i++)
	{
		if (color(i)<0)
			color(i) = 0;
		else if (color(i)>1.f)
			color(i) = 1;
	}
}

void Scene::clip2(Vector3f& color)
{
	for (int i = 0; i<3; i++)
	{
		if (color(i)>10.f)
			color(i) = 10;
	}
}

float Scene::noise(float viewport_scale, float img_scale)
{
	float r = rand() % 10000;
	r = r / 10000;
	return r*viewport_scale / img_scale;
}

void Scene::emitStatus()
{
	//while (computed_row<img_height)
	//	emit statusChange(int(computed_row));
	while (computed_row<img_height)
		pProgressBar->setValue(100.0*computed_row / img_height);
}


//******************set parameters*************************//
void Scene::setViewport(int width, int height)
{
	viewport_height = height;
	viewport_width = width;
}

void Scene::setThreadNum(int num)
{
	thread_num = num;
}
void Scene::setNearplane(int z)
{
	near_plane = z;
}
void Scene::setLightIntensity(int indensity)
{
	light_intensity = indensity;
}
void Scene::setSampleNum(int num)
{
	sample_num = num;
}
void Scene::setMaxDepth(int depth)
{
	max_depth = depth;
}
void Scene::setCameraPos(Vector3f pos)
{
	cam_pos = pos;
}

void Scene::setCameraView(Vector3f view)
{

	cam_view = view.normalized();
}

void Scene::setImgSize(int width, int height)
{
	if (buffer != NULL)
		delete buffer;
	img_width = width;
	img_height = height;
	buffer = new Vector3f[img_height*img_width];
}

void Scene::setMaterial()
{
	m_objects[0]->setMaterial(new Light(light_intensity));
	m_objects[1]->setMaterial(new Diffuse(Vector3f(0.75, 0.25, 0.25)));
	m_objects[2]->setMaterial(new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	m_objects[3]->setMaterial(new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	m_objects[4]->setMaterial(new Diffuse(Vector3f(0.25, 0.25, 0.75)));
	m_objects[5]->setMaterial(new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	m_objects[6]->setMaterial(new Mirror());
	m_objects[7]->setMaterial(new Glass());
}

void Scene::setObjMaterial(int obj_index, Material *m)
{
	if (obj_index<m_objects.size() && obj_index >= 0)
		m_objects[obj_index]->setMaterial(m);
}

//******************get parameters*************************//
int Scene::getCompletedRows()
{
	int num = computed_row;
	return num;
}

int Scene::getTotalRows()
{
	return img_height;
}

float Scene::getLightIntensity()
{
	return light_intensity;
}