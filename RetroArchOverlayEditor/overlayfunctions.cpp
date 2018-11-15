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

void OverlayFunctions::on_setBackground_clicked(){
   ((MainWindow*)parentWidget())->editor->setBackground(ui->imagePath->text());
}
