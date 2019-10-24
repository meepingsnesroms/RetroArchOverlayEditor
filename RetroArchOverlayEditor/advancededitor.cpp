#include "advancededitor.h"
#include "ui_advancededitor.h"

AdvancedEditor::AdvancedEditor(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::AdvancedEditor){
   ui->setupUi(this);
}

AdvancedEditor::~AdvancedEditor(){
   delete ui;
}
