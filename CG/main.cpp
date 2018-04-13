#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <crtdbg.h>
#define MEMORY_LEAK_CHECK _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|\
_CRTDBG_LEAK_CHECK_DF);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	mainwindow w;
	w.show();
	return a.exec();
}
