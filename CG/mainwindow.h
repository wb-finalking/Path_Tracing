#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>
#include <fstream>
#include <Qtimer>
#include "FileReading.h"
#include "Scene.h"

class mainwindow : public QMainWindow
{
	Q_OBJECT

public:
	mainwindow(QWidget *parent = 0);
	~mainwindow();

private :
	void showScene1();
	void showScene2();
	void getParam();

	private slots:
		void getAllObjs();
		void saveImg();
		void LoadScene();
		void render();
		void statusUpdate();
		void setScene1();
		void setScene2();
		void readTXT();
		void setMCSD();
		void setMCBD();
		void modifyStatus(int row);

private:
	Ui::mainwindowClass ui;
	RayTracingType raytracing_type;
	QImage img_scene;
	Scene *scene;
	QLabel *statusLabel;
	QTimer *timer;
	QProgressBar * pProgressBar;
	bool watch;
	std::thread status_check;
};

#endif // MAINWINDOW_H
