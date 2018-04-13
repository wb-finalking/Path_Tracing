/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainwindowClass
{
public:
    QAction *actionObj_Path;
    QAction *actionSave;
    QAction *actionLoad_Scene;
    QWidget *centralWidget;
    QLabel *Scene_Label;
    QPushButton *Render;
    QCheckBox *scene1;
    QCheckBox *scene2;
    QLabel *sampleLabel;
    QLabel *theadLabel;
    QLabel *depthLabel;
    QLabel *lightLabel;
    QLineEdit *lightEdit;
    QLineEdit *sampleEdit;
    QLineEdit *depthEdit;
    QComboBox *threadBox;
    QCheckBox *MCSD;
    QCheckBox *MCBD;
    QLabel *label;
    QLabel *label_2;
    QCheckBox *read_txt;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *mainwindowClass)
    {
        if (mainwindowClass->objectName().isEmpty())
            mainwindowClass->setObjectName(QStringLiteral("mainwindowClass"));
        mainwindowClass->resize(978, 600);
        actionObj_Path = new QAction(mainwindowClass);
        actionObj_Path->setObjectName(QStringLiteral("actionObj_Path"));
        actionSave = new QAction(mainwindowClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionLoad_Scene = new QAction(mainwindowClass);
        actionLoad_Scene->setObjectName(QStringLiteral("actionLoad_Scene"));
        centralWidget = new QWidget(mainwindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        Scene_Label = new QLabel(centralWidget);
        Scene_Label->setObjectName(QStringLiteral("Scene_Label"));
        Scene_Label->setGeometry(QRect(0, 0, 801, 531));
        Render = new QPushButton(centralWidget);
        Render->setObjectName(QStringLiteral("Render"));
        Render->setGeometry(QRect(860, 450, 93, 28));
        scene1 = new QCheckBox(centralWidget);
        scene1->setObjectName(QStringLiteral("scene1"));
        scene1->setGeometry(QRect(860, 300, 91, 19));
        scene2 = new QCheckBox(centralWidget);
        scene2->setObjectName(QStringLiteral("scene2"));
        scene2->setGeometry(QRect(860, 330, 91, 19));
        sampleLabel = new QLabel(centralWidget);
        sampleLabel->setObjectName(QStringLiteral("sampleLabel"));
        sampleLabel->setEnabled(true);
        sampleLabel->setGeometry(QRect(840, 230, 51, 16));
        theadLabel = new QLabel(centralWidget);
        theadLabel->setObjectName(QStringLiteral("theadLabel"));
        theadLabel->setEnabled(true);
        theadLabel->setGeometry(QRect(840, 190, 51, 16));
        depthLabel = new QLabel(centralWidget);
        depthLabel->setObjectName(QStringLiteral("depthLabel"));
        depthLabel->setGeometry(QRect(840, 150, 51, 16));
        lightLabel = new QLabel(centralWidget);
        lightLabel->setObjectName(QStringLiteral("lightLabel"));
        lightLabel->setGeometry(QRect(840, 110, 51, 16));
        lightEdit = new QLineEdit(centralWidget);
        lightEdit->setObjectName(QStringLiteral("lightEdit"));
        lightEdit->setGeometry(QRect(890, 110, 81, 21));
        sampleEdit = new QLineEdit(centralWidget);
        sampleEdit->setObjectName(QStringLiteral("sampleEdit"));
        sampleEdit->setGeometry(QRect(890, 230, 81, 21));
        depthEdit = new QLineEdit(centralWidget);
        depthEdit->setObjectName(QStringLiteral("depthEdit"));
        depthEdit->setEnabled(true);
        depthEdit->setGeometry(QRect(890, 150, 81, 21));
        threadBox = new QComboBox(centralWidget);
        threadBox->setObjectName(QStringLiteral("threadBox"));
        threadBox->setEnabled(true);
        threadBox->setGeometry(QRect(890, 190, 81, 22));
        threadBox->setEditable(false);
        threadBox->setMaxVisibleItems(5);
        threadBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
        threadBox->setFrame(true);
        MCSD = new QCheckBox(centralWidget);
        MCSD->setObjectName(QStringLiteral("MCSD"));
        MCSD->setGeometry(QRect(860, 50, 91, 19));
        MCSD->setChecked(true);
        MCBD = new QCheckBox(centralWidget);
        MCBD->setObjectName(QStringLiteral("MCBD"));
        MCBD->setGeometry(QRect(860, 80, 91, 19));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(841, 20, 131, 20));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(860, 270, 91, 16));
        read_txt = new QCheckBox(centralWidget);
        read_txt->setObjectName(QStringLiteral("read_txt"));
        read_txt->setGeometry(QRect(860, 390, 91, 19));
        mainwindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(mainwindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 978, 26));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        mainwindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(mainwindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainwindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(mainwindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        mainwindowClass->setStatusBar(statusBar);
        QWidget::setTabOrder(Render, scene2);
        QWidget::setTabOrder(scene2, sampleEdit);
        QWidget::setTabOrder(sampleEdit, threadBox);
        QWidget::setTabOrder(threadBox, depthEdit);
        QWidget::setTabOrder(depthEdit, lightEdit);
        QWidget::setTabOrder(lightEdit, scene1);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionObj_Path);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionLoad_Scene);

        retranslateUi(mainwindowClass);

        threadBox->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(mainwindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainwindowClass)
    {
        mainwindowClass->setWindowTitle(QApplication::translate("mainwindowClass", "mainwindow", Q_NULLPTR));
        actionObj_Path->setText(QApplication::translate("mainwindowClass", "Obj Path", Q_NULLPTR));
        actionSave->setText(QApplication::translate("mainwindowClass", "Save", Q_NULLPTR));
        actionLoad_Scene->setText(QApplication::translate("mainwindowClass", "Load Scene", Q_NULLPTR));
        Scene_Label->setText(QApplication::translate("mainwindowClass", "TextLabel", Q_NULLPTR));
        Render->setText(QApplication::translate("mainwindowClass", "Render", Q_NULLPTR));
        scene1->setText(QApplication::translate("mainwindowClass", "Scene1", Q_NULLPTR));
        scene2->setText(QApplication::translate("mainwindowClass", "Scene2", Q_NULLPTR));
        sampleLabel->setText(QApplication::translate("mainwindowClass", "Sample", Q_NULLPTR));
        theadLabel->setText(QApplication::translate("mainwindowClass", "Thread", Q_NULLPTR));
        depthLabel->setText(QApplication::translate("mainwindowClass", "Depth", Q_NULLPTR));
        lightLabel->setText(QApplication::translate("mainwindowClass", "Light", Q_NULLPTR));
        threadBox->clear();
        threadBox->insertItems(0, QStringList()
         << QApplication::translate("mainwindowClass", "1", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "2", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "3", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "4", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "5", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "6", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "7", Q_NULLPTR)
         << QApplication::translate("mainwindowClass", "8", Q_NULLPTR)
        );
        threadBox->setCurrentText(QApplication::translate("mainwindowClass", "4", Q_NULLPTR));
        MCSD->setText(QApplication::translate("mainwindowClass", "MCSD", Q_NULLPTR));
        MCBD->setText(QApplication::translate("mainwindowClass", "MCBD", Q_NULLPTR));
        label->setText(QApplication::translate("mainwindowClass", "RayTracing Type", Q_NULLPTR));
        label_2->setText(QApplication::translate("mainwindowClass", "Scene Demo", Q_NULLPTR));
        read_txt->setText(QApplication::translate("mainwindowClass", "read txt", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("mainwindowClass", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class mainwindowClass: public Ui_mainwindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
