#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "overlayeditor.h"
#include "overlayfunctions.h"

#include <QFileDialog>


#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 240


MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow){
   ui->setupUi(this);

   editor = new OverlayEditor(DEFAULT_WIDTH, DEFAULT_HEIGHT);
   currentOpenFile = "";
   refreshDisplay = new QTimer(this);

   QAction *newAction = new QAction("New", this);
   newAction->setStatusTip("Create a new overlay");
   connect(newAction, &QAction::triggered, this, &MainWindow::new_DERESERVED);
   ui->menuFile->addAction(newAction);

   QAction *openAction = new QAction("Open", this);
   openAction->setStatusTip("Open overlay file");
   connect(openAction, &QAction::triggered, this, &MainWindow::open);
   ui->menuFile->addAction(openAction);

   QAction *saveAction = new QAction("Save", this);
   saveAction->setStatusTip("Save changes");
   connect(saveAction, &QAction::triggered, this, &MainWindow::save);
   ui->menuFile->addAction(saveAction);

   QAction *saveAsAction = new QAction("Save As", this);
   saveAsAction->setStatusTip("Save as a new file");
   connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
   ui->menuFile->addAction(saveAsAction);

   QAction *setBackgroundAction = new QAction("Set Background", this);
   setBackgroundAction->setStatusTip("Change the current background");
   connect(setBackgroundAction, &QAction::triggered, this, &MainWindow::setBackground);
   ui->menuActions->addAction(setBackgroundAction);

   QAction *addButtonAction = new QAction("Add Button", this);
   addButtonAction->setStatusTip("Add a button");
   connect(addButtonAction, &QAction::triggered, this, &MainWindow::addButton);
   ui->menuActions->addAction(addButtonAction);

   QAction *addJoystickAction = new QAction("Add Joystick", this);
   addJoystickAction->setStatusTip("Add a joystick");
   connect(addJoystickAction, &QAction::triggered, this, &MainWindow::addJoystick);
   ui->menuActions->addAction(addJoystickAction);

   connect(refreshDisplay, SIGNAL(timeout()), this, SLOT(updateDisplay()));
   refreshDisplay->start(1000 / 15);//15 FPS

   redraw();
}

MainWindow::~MainWindow(){
   delete refreshDisplay;
   delete editor;
   delete ui;
}

void MainWindow::redraw(){
   //redraw this UI, not the overlay editor
}

void MainWindow::updateDisplay(){
   ui->framebuffer->setPixmap(editor->getImage().scaled(ui->framebuffer->width(), ui->framebuffer->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
   ui->framebuffer->repaint();
}

void MainWindow::new_DERESERVED(){
   editor->setCanvasSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
   editor->reset();
}

void MainWindow::open(){
   QString overlay = QFileDialog::getOpenFileName(this, "Select File", QDir::root().path(), "Overlay (*.cfg)");

   if(overlay != "")
      editor->loadFromFile(overlay);
}

void MainWindow::save(){
   editor->saveToFile(currentOpenFile);
}

void MainWindow::saveAs(){
   QString overlay = QFileDialog::getSaveFileName(this, "Save File", QDir::root().path(), "Overlay (*.cfg)");

   if(overlay != "")
      editor->saveToFile(overlay);
}

void MainWindow::setBackground(){
   QString image = QFileDialog::getOpenFileName(this, "Load Background Image", QDir::root().path(), "Image (*.png *.jpg *.jpeg *.bmp)");

   if(image != "")
      editor->setBackground(image);
}

void MainWindow::addButton(){

}

void MainWindow::addJoystick(){

}

void MainWindow::deleteObjects(){

}

void MainWindow::setCircularObjects(){

}

void MainWindow::setSquareObjects(){

}

void MainWindow::setObjectsCoordinates(){

}

void MainWindow::alignObjectWithBorderPixels(){

}

void MainWindow::advancedEdit(){

}
