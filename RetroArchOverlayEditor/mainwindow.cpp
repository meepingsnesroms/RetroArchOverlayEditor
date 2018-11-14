#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "overlayeditor.h"


MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow){
   ui->setupUi(this);

   editor = new OverlayEditor(ui->framebuffer->width(), ui->framebuffer->height());
   refreshDisplay = new QTimer(this);

   connect(refreshDisplay, SIGNAL(timeout()), this, SLOT(updateDisplay()));
   refreshDisplay->start(1000 / 15);//15 FPS
}

MainWindow::~MainWindow(){
   delete ui;
   delete editor;
}

void MainWindow::updateDisplay(){
   ui->framebuffer->setPixmap(editor->getImage().scaled(ui->framebuffer->width(), ui->framebuffer->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
   ui->framebuffer->repaint();
}
