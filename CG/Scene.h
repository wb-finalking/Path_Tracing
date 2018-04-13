#ifndef _SCENE_
#define _SCENE_
#include <vector>
#include <thread>
#include <atomic>
#include <QImage>
#include <QLabel>
#include <QProgressBar>
#include "object.h"

enum RayTracingType{Ordinary,MCSD,MCBD};

class Object;
class InterPoint;
class Scene : public QObject
{
	Q_OBJECT
public:
	Scene();
	~Scene();
	friend class Material;
	friend class Light;
	friend class Diffuse;
	friend class Mirror;
	friend class Glass;
	friend class Metal;
	Vector3f *buffer;
	static std::atomic_int computed_row;
	QProgressBar * pProgressBar;

	void render(RayTracingType raytracing_type= MCSD);
	void subRender(int start_row, int end_row, RayTracingType raytracing_type = MCSD);
	void loadScene(std::vector<std::string>&obj_files);
	void loadObj(std::string filename);
	void loadSphere(const Vector3f center,const float r);
	void appendMaterial(Material* m);
	void renderViewport(QImage& img_scene);
	void clearObjs();
	
	//set parameters
	void setViewport(int width, int height);
	void setImgSize(int width, int height);
	void setThreadNum(int num);
	void setNearplane(int z);
	void setLightIntensity(int indensity);
	void setSampleNum(int num);
	void setMaxDepth(int depth);
	void setCameraPos(Vector3f pos);
	void setCameraView(Vector3f view);
	void setMaterial();
	void setObjMaterial(int obj_index, Material *m);

	//get parameters
	int getCompletedRows();
	int getTotalRows();
	float getLightIntensity();

signals:
	void statusChange(int row);

private:
	std::vector<Object*> m_objects;
	std::vector<std::thread> m_thread_list;
	Vector3f cam_pos;//position of cam
	Vector3f cam_right;//right of cam
	Vector3f cam_up;//up of cam
	Vector3f cam_view;//forward of cam
	Vector3f environment_color;
	int max_depth;
	int sample_num;
	int light_intensity;
	int viewport_width;
	int viewport_height;
	int img_width;
	int img_height;
	int thread_num;
	float near_plane;
	//static std::atomic_int computed_row;

	void multithread(RayTracingType raytracing_type = MCSD);
	void clip(Vector3f& color);
	void clip2(Vector3f& color);
	void statusUpdate();
	Vector3f RayTracer(const Vector3f& org, const Vector3f& dir);
	bool sceneIntersecting(const Vector3f& org, const Vector3f& dir, InterPoint& inter_p, bool is_out=true);
	Vector3f MCSDRayTracer(const Vector3f& org, const Vector3f& dir, int depth, std::vector<InterPoint>& path,bool refrac=true, bool is_out = true);
	Vector3f MCBDRayTracer(const Vector3f& org, const Vector3f& dir, int depth, bool is_out = true);
	Vector3f pathLinker(std::vector<InterPoint>&view_path, std::vector<InterPoint>&light_path);
	bool pathCheck(std::vector<InterPoint>&view_path, int indexLinked_view, std::vector<InterPoint>&light_path, int indexLinked_light, Vector3f &color, float &path_p);
	
	float noise(float viewport_scale ,float img_scale);

	void emitStatus();

};

#endif