#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "overlayeditor.h"
#include "overlayfunctions.h"


MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow){
   ui->setupUi(this);

   editor = new OverlayEditor(3200, 2400);
   tools = new OverlayFunctions(this);
   refreshDisplay = new QTimer(this);

   tools->show();
   //tools->exec();

   connect(refreshDisplay, SIGNAL(timeout()), this, SLOT(updateDisplay()));
   refreshDisplay->start(1000 / 15);//15 FPS
}

MainWindow::~MainWindow(){
   //refreshDisplay->stop();
   delete refreshDisplay;
   delete tools;
   delete editor;
   delete ui;
}

void MainWindow::updateDisplay(){
   ui->framebuffer->setPixmap(editor->getImage().scaled(ui->framebuffer->width(), ui->framebuffer->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
   ui->framebuffer->repaint();
}
