#include "mainwindow.h"

mainwindow::mainwindow(QWidget *parent)
	: QMainWindow(parent), img_scene(801, 531, QImage::Format_RGB888),
	raytracing_type(MCSD), watch(true)
{
	ui.setupUi(this);
	setFixedSize(978, 600);
	ui.Scene_Label->setFixedSize(801, 531);
	ui.Scene_Label->setPixmap(QPixmap::fromImage(img_scene));

	scene = new Scene();

	connect(ui.actionObj_Path, SIGNAL(triggered()), this, SLOT(getAllObjs()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveImg()));
	connect(ui.actionLoad_Scene, SIGNAL(triggered()), this, SLOT(LoadScene()));
	connect(ui.Render, SIGNAL(clicked()), this, SLOT(render()));
	connect(ui.scene1, SIGNAL(clicked()), this, SLOT(setScene1()));
	connect(ui.scene2, SIGNAL(clicked()), this, SLOT(setScene2()));
	connect(ui.read_txt, SIGNAL(clicked()), this, SLOT(readTXT()));
	connect(ui.MCSD, SIGNAL(clicked()), this, SLOT(setMCSD()));
	connect(ui.MCBD, SIGNAL(clicked()), this, SLOT(setMCBD()));
	connect(scene, SIGNAL(statusChange(int)), this, SLOT(modifyStatus(int)));

	//record status
	pProgressBar = new QProgressBar();
	statusBar()->addWidget(pProgressBar);
	pProgressBar->setRange(0, 100);
	scene->pProgressBar = pProgressBar;
	//pProgressBar = NULL;

	//timer=new QTimer(this);
	//timer->start(1000);
	//statusLabel = new QLabel(this);
	//statusBar()->addWidget(statusLabel);
	//float completed = 0.5;
	//QString str = QString("  %1 completed.......").arg(completed);
	//statusLabel->setText(str);
	//connect(timer, SIGNAL(timeout()), this, SLOT(statusUpdate()));
	//status_check = std::thread(&mainwindow::statusUpdate, this);
}

mainwindow::~mainwindow()
{
	if (scene != NULL)
		delete scene;
	if (statusLabel!=NULL)
		delete statusLabel;
	if (timer != NULL)
		delete timer;
	if (pProgressBar != NULL)
		delete pProgressBar;

	watch = false;
	if (status_check.joinable())
		status_check.join();
}

//raycasting for checking scene
void mainwindow::getAllObjs()
{
	QString file_path = QFileDialog::getExistingDirectory(this, "Select the path of objs...", "../");
	string file_folder = file_path.toStdString();

	vector<string> obj_files;

	getAllFiles(file_folder, obj_files);
	for (string f : obj_files)
		cout << f << endl;

	scene->loadScene(obj_files);

	//scene->loadSphere(Vector3f(-2.3852720 ,  1.6 , 0.43973) ,1.6 );
	//scene->loadSphere(Vector3f(2.24 , 1.6 , 3.9258), 1.6);

	scene->loadSphere(Vector3f(7.10389, 6.43046, 3.03575), 0.897079);
	scene->loadSphere(Vector3f(3.6344, 6.43045, 3.03575), 0.60687);
	scene->loadSphere(Vector3f(-0.0104812, 6.43046, 3.03574), 0.334434);
	scene->loadSphere(Vector3f(-3.36951, 6.43046, 3.03575), 0.114867);

	//std::thread status_check = std::thread(&mainwindow::statusUpdate, this);
	scene->setViewport(6.4, 4);
	scene->setCameraPos(Vector3f(0, 4.95, 30));
	scene->render(Ordinary);
	scene->renderViewport(img_scene);
	ui.Scene_Label->setPixmap(QPixmap::fromImage(img_scene));
	ui.Scene_Label->show();
	//status_check.join();
	

}

void mainwindow::saveImg()
{
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Save Image"),"../",tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.GIF")); 
	if (filename.isEmpty())
	{
		return;
	}
	else
	{
		img_scene.save(filename);	
	}
}

void mainwindow::LoadScene()
{
	QString file_txt = QFileDialog::getOpenFileName(this,
		tr("Load scene"), "../", tr("Image Files (*.txt)"));

	string file_name = file_txt.toStdString();

	std::ifstream file(file_name);
	if (!file.is_open()) {
		std::cout << "Invalid file name!" << std::endl;
	}

	std::string line;
	//make sure not empty
	if (!std::getline(file, line))
		return;
	else
	{
		//get the parameters
		Vector3f cam_pos;
		std::istringstream string_in(line);
		string_in >> cam_pos[0];
		string_in >> cam_pos[1];
		string_in >> cam_pos[2];

		scene->setCameraPos(cam_pos);
	}

	std::getline(file, line);
	int obj_num = std::stoi(line);
	scene->clearObjs();
	for (int i = 0; i < obj_num; i++)
	{
		std::getline(file, line);
		if (line == "Mesh")
		{
			std::getline(file, line);
			std::string obj_name = line;
			scene->loadObj(obj_name);
		}
		else if (line == "Sphere")
		{
			std::getline(file, line);
			std::istringstream string_in(line);
			float radius;
			Vector3f center_pos(0,0,0);
			string_in >> center_pos[0];
			string_in >> center_pos[1];
			string_in >> center_pos[2];
			string_in >> radius;

			scene->loadSphere(center_pos, radius);
		}
		
		std::getline(file, line);
		if (line == "lightsource")
		{
			std::getline(file, line);
			float indensity = std::stof(line);
			scene->appendMaterial(new Light(indensity));
		}
		else if (line == "diffuse")
		{
			std::getline(file, line);
			Vector3f color(0, 0, 0);
			std::istringstream string_in(line);
			string_in >> color[0];
			string_in >> color[1];
			string_in >> color[2];

			scene->appendMaterial(new Diffuse(color));
		}
		else if (line == "mirror")
		{
			scene->appendMaterial(new Mirror());
		}
		else if (line == "glass")
		{
			scene->appendMaterial(new Glass());
		}
		else if (line == "metal")
		{
			std::getline(file, line);
			Vector3f color(0, 0, 0);
			std::istringstream string_in(line);
			string_in >> color[0];
			string_in >> color[1];
			string_in >> color[2];

			std::getline(file, line);
			string_in.str(line);
			float alpha_x, alpha_y;
			string_in >> alpha_x;
			string_in >> alpha_y;

			scene->appendMaterial(new Metal(color, alpha_x, alpha_y));
		}

	}

	//std::istringstream stringin(sub_line);

}

void mainwindow::render()
{

	if (ui.scene1->checkState() == Qt::Checked)
	{
		showScene1();

		//std::thread status_check = std::thread(&mainwindow::statusUpdate, this);
		scene->render(raytracing_type);
		scene->renderViewport(img_scene);
		ui.Scene_Label->setPixmap(QPixmap::fromImage(img_scene));
		ui.Scene_Label->show();
		//status_check.join();
	}
	else if (ui.scene2->checkState() == Qt::Checked)
	{
		showScene2();

		//std::thread status_check = std::thread(&mainwindow::statusUpdate, this);
		scene->render(raytracing_type);
		scene->renderViewport(img_scene);
		ui.Scene_Label->setPixmap(QPixmap::fromImage(img_scene));
		ui.Scene_Label->show();
		//status_check.join();
	}
	else if (ui.read_txt->checkState() == Qt::Checked)
	{
		getParam();
		scene->setViewport(6.4, 4);
		scene->setNearplane(10);
		scene->setCameraView(Vector3f(0, 0, -1));

		scene->render(raytracing_type);
		scene->renderViewport(img_scene);
		ui.Scene_Label->setPixmap(QPixmap::fromImage(img_scene));
		ui.Scene_Label->show();
	}

}

void mainwindow::setScene1()
{
	ui.scene1->setCheckState(Qt::Checked);
	ui.scene2->setCheckState(Qt::Unchecked);
	ui.read_txt->setCheckState(Qt::Unchecked);

	ui.depthEdit->setText("7");
	ui.sampleEdit->setText("64");
	ui.lightEdit->setText("35");
}

void mainwindow::setScene2()
{
	ui.scene1->setCheckState(Qt::Unchecked);
	ui.scene2->setCheckState(Qt::Checked);
	ui.read_txt->setCheckState(Qt::Unchecked);

	ui.depthEdit->setText("7");
	ui.sampleEdit->setText("1");
	ui.lightEdit->setText("35");
}

void mainwindow::readTXT()
{
	ui.scene1->setCheckState(Qt::Unchecked);
	ui.scene2->setCheckState(Qt::Unchecked);
	ui.read_txt->setCheckState(Qt::Checked);

	ui.depthEdit->setText("7");
	ui.sampleEdit->setText("64");
	ui.lightEdit->setText("35");
}

void mainwindow::setMCSD()
{
	ui.MCSD->setCheckState(Qt::Checked);
	ui.MCBD->setCheckState(Qt::Unchecked);
	raytracing_type = MCSD;
}

void mainwindow::setMCBD()
{
	ui.MCSD->setCheckState(Qt::Unchecked);
	ui.MCBD->setCheckState(Qt::Checked);
	raytracing_type = MCBD;
}

void mainwindow::modifyStatus(int row)
{
	float completed = 100.0*row / scene->getTotalRows();
	pProgressBar->setValue(completed);
}

void mainwindow::statusUpdate()
{
	//while (scene->getCompletedRows() < scene->getTotalRows())
	//{
	//	float completed = 100.0*scene->getCompletedRows() / scene->getTotalRows();
	//	pProgressBar->setValue(completed);
	//}
	while (watch)
	{
		float completed = 100.0*scene->computed_row / scene->getTotalRows();
		pProgressBar->setValue(completed);
	}
	pProgressBar->setValue(100);
	return;
}

void mainwindow::showScene1()
{
	scene->setViewport(6.4, 4);
	scene->setCameraPos(Vector3f(0, 4.95, 30));
	scene->setCameraView(Vector3f(0, 0, -1));
	scene->setLightIntensity(35);
	scene->setNearplane(10);
	scene->setThreadNum(4);
	scene->setMaxDepth(7);
	scene->setSampleNum(10000);
	getParam();

	vector<string> obj_files;
	obj_files.push_back("../scene/1/lightsource.obj");
	obj_files.push_back("../scene/1/wall1.obj");
	obj_files.push_back("../scene/1/wall2.obj");
	obj_files.push_back("../scene/1/wall3.obj");
	obj_files.push_back("../scene/1/wall4.obj");
	obj_files.push_back("../scene/1/wall5.obj");

	scene->loadScene(obj_files);

	scene->loadSphere(Vector3f(-2.3852720, 1.6, 0.43973), 1.6);
	scene->loadSphere(Vector3f(3.04, 1.6, 1.9258), 1.6);

	scene->setObjMaterial( 0, new Light(scene->getLightIntensity()));
	scene->setObjMaterial( 1, new Diffuse(Vector3f(0.75, 0.25, 0.25)));
	scene->setObjMaterial( 2, new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	scene->setObjMaterial( 3, new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	scene->setObjMaterial( 4, new Diffuse(Vector3f(0.25, 0.25, 0.75)));
	scene->setObjMaterial( 5, new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	scene->setObjMaterial( 6, new Mirror());
	scene->setObjMaterial( 7, new Glass());
}

void mainwindow::showScene2()
{
	scene->setViewport(8, 5);
	scene->setCameraPos(Vector3f(0, 10, 30));
	scene->setCameraView(Vector3f(0, -2, -6));
	scene->setLightIntensity(35);
	scene->setNearplane(10);
	scene->setThreadNum(4);
	scene->setMaxDepth(7);
	scene->setSampleNum(10000);
	getParam();

	vector<string> obj_files;
	obj_files.push_back("../scene/2/plate1.obj");
	obj_files.push_back("../scene/2/plate2.obj");
	obj_files.push_back("../scene/2/plate3.obj");
	obj_files.push_back("../scene/2/plate4.obj");
	obj_files.push_back("../scene/2/wall1.obj");
	obj_files.push_back("../scene/2/wall2.obj");

	scene->loadScene(obj_files);

	scene->loadSphere(Vector3f(7.10389, 5.43046, 3.03575), 0.897079);
	scene->loadSphere(Vector3f(3.6344, 5.43045, 3.03575), 0.60687);
	scene->loadSphere(Vector3f(-0.0104812, 5.43046, 3.03574), 0.334434);
	scene->loadSphere(Vector3f(-3.36951, 5.43046, 3.03575), 0.114867);

	scene->setObjMaterial(0, new Metal(Vector3f(0.85, 0.85, 0.95), 0.1, 0.005));
	scene->setObjMaterial(1, new Metal(Vector3f(0.85, 0.85, 0.95), 0.1, 0.005));
	scene->setObjMaterial(2, new Metal(Vector3f(0.85, 0.85, 0.95), 0.05, 0.005));
	scene->setObjMaterial(3, new Metal(Vector3f(0.85, 0.85, 0.95), 0.04, 0.005));
	scene->setObjMaterial(4, new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	scene->setObjMaterial(5, new Diffuse(Vector3f(0.25, 0.25, 0.25)));
	scene->setObjMaterial(6, new Light(scene->getLightIntensity()));
	scene->setObjMaterial(7, new Light(scene->getLightIntensity()));
	scene->setObjMaterial(8, new Light(scene->getLightIntensity()));
	scene->setObjMaterial(9, new Light(scene->getLightIntensity()));
}

void mainwindow::getParam()
{
	int thread_num = ui.threadBox->currentText().toInt();
	int sample = ui.sampleEdit->text().toInt();
	int depth = ui.depthEdit->text().toInt();
	float light = ui.lightEdit->text().toFloat();

	scene->setLightIntensity(light);
	scene->setThreadNum(thread_num);
	scene->setMaxDepth(depth);
	scene->setSampleNum(sample);

}

