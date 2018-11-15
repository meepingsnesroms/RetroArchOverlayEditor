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
   if(!ui->newButtonName->text().isEmpty() && !ui->newButtonImagePath->text().isEmpty() && QFile(ui->newButtonImagePath->text()).exists()){
      ((MainWindow*)parentWidget())->editor->add(ui->newButtonName->text(), QPixmap(ui->newButtonImagePath->text()));

      ui->newButtonName->clear();
      ui->newButtonImagePath->clear();
   }
}
