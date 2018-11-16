#include "overlayfunctions.h"
#include "ui_overlayfunctions.h"

#include <QFile>

#include "mainwindow.h"
#include "overlayeditor.h"


OverlayFunctions::OverlayFunctions(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::OverlayFunctions){
   ui->setupUi(this);

   //this window is not closeable, minimizable or fullscreen capable
   setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

   ui->setLayer->setMinimum(0);
   ui->setLayer->setMaximum(0);

   ui->buttonSize->setValue(50);
}

OverlayFunctions::~OverlayFunctions(){
   delete ui;
}

void OverlayFunctions::on_addButton_clicked(){
   ((MainWindow*)parentWidget())->editor->add(ui->buttonName->text(), ui->imagePath->text());
}

void OverlayFunctions::on_deleteButton_clicked(){
   ((MainWindow*)parentWidget())->editor->remove();
}

void OverlayFunctions::on_buttonSetRadial_clicked(){
   ((MainWindow*)parentWidget())->editor->setCollisionType(true);
}

void OverlayFunctions::on_buttonSetRectangular_clicked(){
   ((MainWindow*)parentWidget())->editor->setCollisionType(false);
}

void OverlayFunctions::on_setLayer_valueChanged(int arg1){
   ((MainWindow*)parentWidget())->editor->setLayer(arg1);
}

void OverlayFunctions::on_setBackground_clicked(){
   ((MainWindow*)parentWidget())->editor->setBackground(ui->imagePath->text());
}

void OverlayFunctions::on_newOverlay_clicked(){
   ((MainWindow*)parentWidget())->editor->reset();
}

void OverlayFunctions::on_saveOverlay_clicked(){
   ((MainWindow*)parentWidget())->editor->saveToFile(ui->overlayPath->text());
}

void OverlayFunctions::on_loadOverlay_clicked(){
   ((MainWindow*)parentWidget())->editor->loadFromFile(ui->overlayPath->text());
   ui->setLayer->setMaximum(((MainWindow*)parentWidget())->editor->getTotalLayers() - 1);
}
