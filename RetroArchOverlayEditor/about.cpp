#include "about.h"
#include "ui_about.h"

#include <QIcon>
#include <QPixmap>

About::About(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::About){
   ui->setupUi(this);

   // remove question mark from the title bar
   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

   //make logos draw properly
   ui->logo1->installEventFilter(this);
   ui->logo2->installEventFilter(this);
   ui->logo1->setObjectName("logo1");
   ui->logo2->setObjectName("logo2");
}

About::~About(){
   delete ui;
}

bool About::eventFilter(QObject* object, QEvent* event){
   if(event->type() == QEvent::Resize){
      if(object->objectName() == "logo1")
         ui->logo1->setPixmap(QIcon(":/assets/retroarch.svg").pixmap((float)ui->logo1->width() * 0.90, (float)ui->logo1->height() * 0.90));
      else if(object->objectName() == "logo2")
         ui->logo2->setPixmap(QPixmap(":/assets/smallBunRounded.png").scaled((float)ui->logo2->width() * 0.90, (float)ui->logo2->height() * 0.90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   }

   return QDialog::eventFilter(object, event);
}
