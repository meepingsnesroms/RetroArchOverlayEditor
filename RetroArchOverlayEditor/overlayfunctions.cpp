#include "overlayfunctions.h"
#include "ui_overlayfunctions.h"


OverlayFunctions::OverlayFunctions(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::OverlayFunctions){
   ui->setupUi(this);
}

OverlayFunctions::~OverlayFunctions(){
   delete ui;
}
