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
   ui->expandGroup->setValue(50);
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

void OverlayFunctions::on_buttonSize_sliderReleased(){
   double scaler = (double)ui->buttonSize->value() / 50.0;

   //convert range to 0.5<->2.0
   if(scaler < 1.0){
      scaler /= 2.0;
      scaler += 0.5;
   }

   ((MainWindow*)parentWidget())->editor->resize(scaler, scaler);
   ui->buttonSize->setValue(50);
}

void OverlayFunctions::on_expandGroup_sliderReleased(){
   double scaler = (double)ui->expandGroup->value() / 50.0;

   //convert range to 0.5<->2.0
   if(scaler < 1.0){
      scaler /= 2.0;
      scaler += 0.5;
   }

   ((MainWindow*)parentWidget())->editor->resizeGroupSpacing(scaler, scaler);
   ui->expandGroup->setValue(50);
}

void OverlayFunctions::on_setLayer_valueChanged(int value){
   ((MainWindow*)parentWidget())->editor->setLayer(value);
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
