#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "overlayeditor.h"
#include "overlayfunctions.h"

#include <QFileDialog>


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480


MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow){
   ui->setupUi(this);

   editor = new OverlayEditor(DEFAULT_WIDTH, DEFAULT_HEIGHT);
   currentOpenFile = "";
   sizeSliderLastPostion = 50;
   widthSliderLastPostion = 50;
   heightSliderLastPostion = 50;
   refreshDisplay = new QTimer(this);

   //resize framebuffer properly
   ui->centralWidget->installEventFilter(this);
   ui->centralWidget->setObjectName("centralWidget");

   QAction* newAction = new QAction("New", this);
   newAction->setStatusTip("Create a new overlay");
   connect(newAction, &QAction::triggered, this, &MainWindow::new_DERESERVED);
   ui->menuFile->addAction(newAction);

   QAction* openAction = new QAction("Open", this);
   openAction->setStatusTip("Open overlay file");
   connect(openAction, &QAction::triggered, this, &MainWindow::open);
   ui->menuFile->addAction(openAction);

   QAction* saveAction = new QAction("Save", this);
   saveAction->setStatusTip("Save changes");
   connect(saveAction, &QAction::triggered, this, &MainWindow::save);
   ui->menuFile->addAction(saveAction);

   QAction* saveAsAction = new QAction("Save As", this);
   saveAsAction->setStatusTip("Save as a new file");
   connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
   ui->menuFile->addAction(saveAsAction);

   QAction* setBackgroundAction = new QAction("Set Background", this);
   setBackgroundAction->setStatusTip("Change the current background");
   connect(setBackgroundAction, &QAction::triggered, this, &MainWindow::setBackground);
   ui->menuActions->addAction(setBackgroundAction);

   QAction* addButtonAction = new QAction("Add Button", this);
   addButtonAction->setStatusTip("Add a button");
   connect(addButtonAction, &QAction::triggered, this, &MainWindow::addButton);
   ui->menuActions->addAction(addButtonAction);

   QAction* addJoystickAction = new QAction("Add Joystick", this);
   addJoystickAction->setStatusTip("Add a joystick");
   connect(addJoystickAction, &QAction::triggered, this, &MainWindow::addJoystick);
   ui->menuActions->addAction(addJoystickAction);

   QAction* deleteObjectsAction = new QAction("Delete Object(s)", this);
   deleteObjectsAction->setStatusTip("Delete the selected objects");
   connect(deleteObjectsAction, &QAction::triggered, this, &MainWindow::deleteObjects);
   ui->menuActions->addAction(deleteObjectsAction);

   QAction* setObjectNameAction = new QAction("Set Object Name", this);
   setObjectNameAction->setStatusTip("Set the internal name of object");
   connect(setObjectNameAction, &QAction::triggered, this, &MainWindow::setObjectName);
   ui->menuActions->addAction(setObjectNameAction);

   QAction* setObjectImageAction = new QAction("Set Object Image", this);
   setObjectImageAction->setStatusTip("Set image displayed for object");
   connect(setObjectImageAction, &QAction::triggered, this, &MainWindow::setObjectImage);
   ui->menuActions->addAction(setObjectImageAction);

   QAction* setCircularObjectsAction = new QAction("Set Circular Object(s)", this);
   setCircularObjectsAction->setStatusTip("Make the selected objects hitbox circular");
   connect(setCircularObjectsAction, &QAction::triggered, this, &MainWindow::setCircularObjects);
   ui->menuActions->addAction(setCircularObjectsAction);

   QAction* setSquareObjectsAction = new QAction("Set Square Object(s)", this);
   setSquareObjectsAction->setStatusTip("Make the selected objects hitbox square");
   connect(setSquareObjectsAction, &QAction::triggered, this, &MainWindow::setSquareObjects);
   ui->menuActions->addAction(setSquareObjectsAction);

   QAction* setObjectsCoordinatesAction = new QAction("Set Object(s) Coordinates", this);
   setObjectsCoordinatesAction->setStatusTip("Directly set the coordinates of the selected objects");
   connect(setObjectsCoordinatesAction, &QAction::triggered, this, &MainWindow::setObjectsCoordinates);
   ui->menuActions->addAction(setObjectsCoordinatesAction);

   QAction* alignObjectWithBorderPixelsAction = new QAction("Align Object With Border Pixels", this);
   alignObjectWithBorderPixelsAction->setStatusTip("Match outer 1 pixel border to of object to background and move object there");
   connect(alignObjectWithBorderPixelsAction, &QAction::triggered, this, &MainWindow::alignObjectWithBorderPixels);
   ui->menuActions->addAction(alignObjectWithBorderPixelsAction);

   QAction* advancedEditAction = new QAction("Advanced Edit", this);
   advancedEditAction->setStatusTip("Directly edit overlay object code");
   connect(advancedEditAction, &QAction::triggered, this, &MainWindow::advancedEdit);
   ui->menuActions->addAction(advancedEditAction);

   QAction* aboutAction = new QAction("About", this);
   aboutAction->setStatusTip("About this application");
   connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
   ui->menuAbout->addAction(aboutAction);

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
   ui->menuLayer->clear();
   for(int index = 0; index < editor->getTotalLayers(); index++){
      QAction* layerSelectAction = new QAction(QString::number(index), this);
      connect(layerSelectAction, &QAction::triggered, this, &MainWindow::about);
      ui->menuLayer->addAction(layerSelectAction);
   }
}


bool MainWindow::eventFilter(QObject* object, QEvent* event){
   if(event->type() == QEvent::Resize && object->objectName() == "centralWidget"){
      float wR = 1.0;
      float hR = (float)editor->getImage().height() / (float)editor->getImage().width();
      float smallestRatio;

      //update displayContainer first, make the display occupy the top 3/5 of the screen if there are Palm keys or 4/5 if theres not
      ui->displayContainer->setFixedHeight(ui->centralWidget->height() * 0.80);

      smallestRatio = qMin(ui->displayContainer->size().width() * 0.98 / wR, ui->displayContainer->size().height() * 0.98 / hR);
      //the 0.98 above allows the display to shrink, without it the displayContainer couldnt shrink because of the fixed size of the display

      //set new size
      ui->framebuffer->setFixedSize(smallestRatio * wR, smallestRatio * hR);

      //scale framebuffer to new size and refresh
      ui->framebuffer->repaint();
   }

   return QMainWindow::eventFilter(object, event);
}

void MainWindow::updateDisplay(){
   ui->framebuffer->setPixmap(editor->getImage().scaled(ui->framebuffer->width(), ui->framebuffer->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
   ui->framebuffer->repaint();
}

void MainWindow::new_DERESERVED(){
   editor->setCanvasSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
   editor->reset();
   redraw();
}

void MainWindow::open(){
   QString overlay = QFileDialog::getOpenFileName(this, "Select File", QDir::root().path(), "Overlay (*.cfg)");

   if(overlay != ""){
      editor->loadFromFile(overlay);
      redraw();
   }
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
   editor->addButton();
}

void MainWindow::addJoystick(){
   editor->addJoystick();
}

void MainWindow::deleteObjects(){
   editor->remove();
}

void MainWindow::setObjectName(){

}

void MainWindow::setObjectImage(){
   QString image = QFileDialog::getOpenFileName(this, "Load Object Image", QDir::root().path(), "Image (*.png *.jpg *.jpeg *.bmp)");

   if(image != "")
      editor->setObjectImage(image);
}

void MainWindow::setCircularObjects(){
   editor->setCollisionType(true);
}

void MainWindow::setSquareObjects(){
   editor->setCollisionType(false);
}

void MainWindow::setObjectsCoordinates(){

}

void MainWindow::alignObjectWithBorderPixels(){

}

void MainWindow::advancedEdit(){
   //TODO: need to save file to temp, load it in an editor and when its closed reload from temp
}

void MainWindow::about(){

}

void MainWindow::on_sizeSlider_sliderMoved(int position){

}

void MainWindow::on_sizeSlider_sliderReleased(){
   sizeSliderLastPostion = 50;
}

void MainWindow::on_widthSlider_sliderMoved(int position){

}

void MainWindow::on_widthSlider_sliderReleased(){
   widthSliderLastPostion = 50;
}

void MainWindow::on_heightSlider_sliderMoved(int position){

}

void MainWindow::on_heightSlider_sliderReleased(){
   heightSliderLastPostion = 50;
}
