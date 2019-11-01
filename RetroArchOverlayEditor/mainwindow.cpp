#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "overlayeditor.h"

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480


MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow){
   ui->setupUi(this);

   editor = new OverlayEditor(DEFAULT_WIDTH, DEFAULT_HEIGHT);
   sizeSliderLastPostion = 50;
   widthSliderLastPostion = 50;
   heightSliderLastPostion = 50;
   spreadSliderLastPostion = 50;
   refreshDisplay = new QTimer(this);

   //resize framebuffer properly
   ui->centralWidget->installEventFilter(this);
   ui->centralWidget->setObjectName("centralWidget");

   QAction* newAction = new QAction("New", this);
   newAction->setStatusTip("Create a new overlay");
   newAction->setShortcuts(QKeySequence::New);
   connect(newAction, &QAction::triggered, this, &MainWindow::new_DERESERVED);
   ui->menuFile->addAction(newAction);

   QAction* openAction = new QAction("Open", this);
   openAction->setStatusTip("Open overlay file");
   openAction->setShortcuts(QKeySequence::Open);
   connect(openAction, &QAction::triggered, this, &MainWindow::open);
   ui->menuFile->addAction(openAction);

   QAction* saveAction = new QAction("Save", this);
   saveAction->setStatusTip("Save changes");
   saveAction->setShortcuts(QKeySequence::Save);
   connect(saveAction, &QAction::triggered, this, &MainWindow::save);
   ui->menuFile->addAction(saveAction);

   QAction* saveAsAction = new QAction("Save As", this);
   saveAsAction->setStatusTip("Save as a new file");
   saveAsAction->setShortcuts(QKeySequence::SaveAs);
   connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
   ui->menuFile->addAction(saveAsAction);

   QAction* setCanvasSizeAction = new QAction("Set Canvas Size", this);
   setCanvasSizeAction->setStatusTip("Change the size of the framebuffer being rendered to");
   connect(setCanvasSizeAction, &QAction::triggered, this, &MainWindow::setCanvasSize);
   ui->menuActions->addAction(setCanvasSizeAction);

   QAction* setBackgroundAction = new QAction("Set Background", this);
   setBackgroundAction->setStatusTip("Change the current background");
   connect(setBackgroundAction, &QAction::triggered, this, &MainWindow::setBackground);
   ui->menuActions->addAction(setBackgroundAction);

   QAction* removeBackgroundAction = new QAction("Remove Background", this);
   removeBackgroundAction->setStatusTip("Remove the current background");
   connect(removeBackgroundAction, &QAction::triggered, this, &MainWindow::removeBackground);
   ui->menuActions->addAction(removeBackgroundAction);

   QAction* addLayerAction = new QAction("Add Layer", this);
   addLayerAction->setStatusTip("Add another layer to the overlay");
   connect(addLayerAction, &QAction::triggered, this, &MainWindow::addLayer);
   ui->menuActions->addAction(addLayerAction);

   QAction* deleteLayerAction = new QAction("Delete Layer", this);
   deleteLayerAction->setStatusTip("Delete the current layer from the overlay");
   connect(deleteLayerAction, &QAction::triggered, this, &MainWindow::deleteLayer);
   ui->menuActions->addAction(deleteLayerAction);

   QAction* setLayerPropertiesAction = new QAction("Set Layer Properties", this);
   setLayerPropertiesAction->setStatusTip("Set misc properties for the current layer");
   connect(setLayerPropertiesAction, &QAction::triggered, this, &MainWindow::setLayerProperties);
   ui->menuActions->addAction(setLayerPropertiesAction);

   QAction* setLayerImageAction = new QAction("Set Layer Image", this);
   setLayerImageAction->setStatusTip("Set layer image for overlay");
   connect(setLayerImageAction, &QAction::triggered, this, &MainWindow::setLayerImage);
   ui->menuActions->addAction(setLayerImageAction);

   QAction* removeLayerImageAction = new QAction("Remove Layer Image", this);
   removeLayerImageAction->setStatusTip("Remove layer image from overlay");
   connect(removeLayerImageAction, &QAction::triggered, this, &MainWindow::removeLayerImage);
   ui->menuActions->addAction(removeLayerImageAction);

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

   QAction* setObjectPropertiesAction = new QAction("Set Object Properties", this);
   setObjectPropertiesAction->setStatusTip("Set misc properties of an object");
   connect(setObjectPropertiesAction, &QAction::triggered, this, &MainWindow::setObjectProperties);
   ui->menuActions->addAction(setObjectPropertiesAction);

   QAction* setObjectImageAction = new QAction("Set Object Image", this);
   setObjectImageAction->setStatusTip("Set image displayed for selected object");
   connect(setObjectImageAction, &QAction::triggered, this, &MainWindow::setObjectImage);
   ui->menuActions->addAction(setObjectImageAction);

   QAction* removeObjectImageAction = new QAction("Remove Object Image", this);
   removeObjectImageAction->setStatusTip("Remove the image attached to the selected object");
   connect(removeObjectImageAction, &QAction::triggered, this, &MainWindow::removeObjectImage);
   ui->menuActions->addAction(removeObjectImageAction);

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

   QAction* setObjectsSizeAction = new QAction("Set Object(s) Size", this);
   setObjectsSizeAction->setStatusTip("Directly set the size of the selected object or object group");
   connect(setObjectsSizeAction, &QAction::triggered, this, &MainWindow::setObjectsSize);
   ui->menuActions->addAction(setObjectsSizeAction);

   QAction* pluckObjectsImageFromLayerImageAction = new QAction("Pluck Object(s) Image From Layer Image", this);
   pluckObjectsImageFromLayerImageAction->setStatusTip("Take object images from the layer background, can be edited later to make animated overlays");
   connect(pluckObjectsImageFromLayerImageAction, &QAction::triggered, this, &MainWindow::pluckObjectsImageFromLayerImage);
   ui->menuActions->addAction(pluckObjectsImageFromLayerImageAction);

   QAction* aboutAction = new QAction("About", this);
   aboutAction->setStatusTip("About this application");
   connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
   ui->menuAbout->addAction(aboutAction);

   connect(refreshDisplay, SIGNAL(timeout()), this, SLOT(updateDisplay()));
   refreshDisplay->start(1000 / 30);//30 FPS

   redraw();
}

MainWindow::~MainWindow(){
   delete refreshDisplay;
   delete editor;
   delete ui;
}

void MainWindow::redraw(){
   QResizeEvent* resizeEvent = new QResizeEvent(ui->centralWidget->size(), ui->centralWidget->size());

   //force update current size
   QCoreApplication::postEvent(ui->centralWidget, resizeEvent);

   //redraw this UI, not the overlay editor
   ui->menuLayer->clear();
   for(int index = 0; index < editor->getTotalLayers(); index++){
      QAction* layerSelectAction = new QAction(QString::number(index), this);
      layerSelectAction->setProperty("layer_num", index);
      layerSelectAction->setCheckable(index == editor->getLayer());
      layerSelectAction->setChecked(index == editor->getLayer());
      connect(layerSelectAction, &QAction::triggered, this, &MainWindow::layerChange);
      ui->menuLayer->addAction(layerSelectAction);
   }
}

bool MainWindow::eventFilter(QObject* object, QEvent* event){
   if(event->type() == QEvent::Resize && object->objectName() == "centralWidget"){
      float wR = 1.0;
      float hR = (float)editor->getImage().height() / (float)editor->getImage().width();
      float smallestRatio;

      //force how much of the window the screen occupies
      ui->displayContainer->setFixedHeight(ui->centralWidget->height() * 0.70);

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

void MainWindow::handleErrorCode(const QString& function, const QString& errorCode){
   if(!errorCode.isEmpty())
      QMessageBox::information(this, function, errorCode);
}

void MainWindow::new_DERESERVED(){
   editor->setCanvasSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
   editor->reset();
   redraw();
}

void MainWindow::open(){
   QString overlay = QFileDialog::getOpenFileName(this, "Select File", QDir::root().path(), "Overlay (*.cfg)");

   if(!overlay.isEmpty()){
      handleErrorCode("Load Overlay", editor->loadFromFile(overlay));
      redraw();
   }
}

void MainWindow::save(){
   QString currentOverlay = editor->getCurrentlyOpenOverlay();

   if(!currentOverlay.isEmpty())
      handleErrorCode("Save Overlay", editor->saveToFile(currentOverlay));
}

void MainWindow::saveAs(){
   QString overlay = QFileDialog::getSaveFileName(this, "Save File", QFileInfo(editor->getCurrentlyOpenOverlay()).path(), "Overlay (*.cfg)");

   if(!overlay.isEmpty())
     handleErrorCode("Save Overlay",  editor->saveToFile(overlay));
}

void MainWindow::setCanvasSize(){
   bool ok;
   QString widthHeight;
   int width;
   int height;

   editor->getCanvasSize(&width, &height);

   widthHeight = QInputDialog::getText(this, "Set Canvas Size", "New Canvas Size:", QLineEdit::Normal, QString::number(width) + ", " + QString::number(height), &ok);

   if(ok){
      QStringList widthHeightSplit = widthHeight.split(",");

      if(widthHeightSplit.size() == 2){
         bool ok[2];

         width = widthHeightSplit[0].toInt(&ok[0]);
         height = widthHeightSplit[1].toInt(&ok[1]);

         if(ok[0] && ok[1] && width > 0 && height > 0){
            refreshDisplay->stop();
            //dont render when canvas is null
            editor->setCanvasSize(width, height);
            redraw();
            refreshDisplay->start();
         }
      }
   }
}

void MainWindow::setBackground(){
   QString image = QFileDialog::getOpenFileName(this, "Choose Background Image", QFileInfo(editor->getCurrentlyOpenOverlay()).path(), "Image (*.png *.jpg *.jpeg *.bmp)");

   if(!image.isEmpty())
      editor->setBackground(image);
}

void MainWindow::removeBackground(){
   editor->setBackground("");
}

void MainWindow::addLayer(){
   editor->newLayer();
   redraw();
}

void MainWindow::deleteLayer(){
   editor->removeLayer(editor->getLayer());
   redraw();
}

void MainWindow::setLayerProperties(){
   bool ok;
   QString properties = QInputDialog::getText(this, "Set Layer Properties", "Layer Properties:", QLineEdit::Normal, editor->getLayerProperties(editor->getLayer()), &ok);

   if(ok)
      editor->setLayerProperties(editor->getLayer(), properties);
}

void MainWindow::setLayerImage(){
   QString image = QFileDialog::getOpenFileName(this, "Choose Layer Image", QFileInfo(editor->getCurrentlyOpenOverlay()).path(), "Image (*.png *.jpg *.jpeg *.bmp)");

   if(!image.isEmpty())
      editor->setLayerImage(image);
}

void MainWindow::removeLayerImage(){
   editor->setLayerImage("");
}

void MainWindow::addButton(){
   editor->addObject(false);
}

void MainWindow::addJoystick(){
   editor->addObject(true);
}

void MainWindow::deleteObjects(){
   editor->remove();
}

void MainWindow::setObjectName(){
   if(editor->singleObjectSelected()){
      bool ok;
      QString name = QInputDialog::getText(this, "Choose Object Name", "Object Name:", QLineEdit::Normal, editor->getObjectName(), &ok);

      if(ok)
         handleErrorCode("Set Object Name", editor->setObjectName(name));
   }
}

void MainWindow::setObjectProperties(){
   if(editor->singleObjectSelected()){
      bool ok;
      QString properties = QInputDialog::getText(this, "Set Object Properties", "Object Properties:", QLineEdit::Normal, editor->getObjectProperties(), &ok);

      if(ok)
         handleErrorCode("Set Object Properties", editor->setObjectProperties(properties));
   }
}

void MainWindow::setObjectImage(){
   if(editor->singleObjectSelected()){
      QString image = QFileDialog::getOpenFileName(this, "Choose Object Image", QFileInfo(editor->getCurrentlyOpenOverlay()).path(), "Image (*.png *.jpg *.jpeg *.bmp)");

      if(!image.isEmpty())
         editor->setObjectImage(image);
   }
}

void MainWindow::removeObjectImage(){
   if(editor->singleObjectSelected())
      editor->setObjectImage("");
}

void MainWindow::setCircularObjects(){
   editor->setCollisionType(true);
}

void MainWindow::setSquareObjects(){
   editor->setCollisionType(false);
}

void MainWindow::setObjectsCoordinates(){
   if(editor->singleObjectSelected() || editor->multipleObjectsSelected()){
      bool ok;
      QString coordinates;
      double x;
      double y;

      editor->getObjectsCoordinates(&x, &y);

      coordinates = QInputDialog::getText(this, "Set Object(s) Coordinates", "Object(s) New Coordinates:", QLineEdit::Normal, QString::number(x) + ", " + QString::number(y), &ok);

      if(ok){
         QStringList coordinatesSplit = coordinates.split(",");

         if(coordinatesSplit.size() == 2){
            bool ok[2];

            x = coordinatesSplit[0].toDouble(&ok[0]);
            y = coordinatesSplit[1].toDouble(&ok[1]);

            if(ok[0] && ok[1])
               editor->setObjectsCoordinates(x, y);
         }
      }
   }
}

void MainWindow::setObjectsSize(){
   if(editor->singleObjectSelected() || editor->multipleObjectsSelected()){
      bool ok;
      QString widthHeight;
      double width;
      double height;

      editor->getObjectsSize(&width, &height);

      widthHeight = QInputDialog::getText(this, "Set Object(s) Size", "Object(s) New Size:", QLineEdit::Normal, QString::number(width) + ", " + QString::number(height), &ok);

      if(ok){
         QStringList widthHeightSplit = widthHeight.split(",");

         if(widthHeightSplit.size() == 2){
            bool ok[2];

            width = widthHeightSplit[0].toDouble(&ok[0]);
            height = widthHeightSplit[1].toDouble(&ok[1]);

            if(ok[0] && ok[1])
               editor->setObjectsSize(width, height);
         }
      }
   }
}

void MainWindow::pluckObjectsImageFromLayerImage(){
   handleErrorCode("Pluck Object(s) Image From Layer Image", editor->pluckObjectsImageFromLayerImage());
}

void MainWindow::layerChange(){
   editor->setLayer(sender()->property("layer_num").toInt());
   redraw();
}

void MainWindow::about(){
   aboutWindow.exec();
}

void MainWindow::on_sizeSlider_sliderMoved(int position){
   double multiplier = (float)(position - sizeSliderLastPostion) / 50.0 + 1.0;

   if(ui->spreadEnabled->isChecked())
      editor->resizeGroupSpacing(multiplier, multiplier);
   else
      editor->resize(multiplier, multiplier);
   sizeSliderLastPostion = position;
}

void MainWindow::on_sizeSlider_valueChanged(int value){
   //have to handle this contition to support keyboard events
    if(value != sizeSliderLastPostion){
       double multiplier = (float)(value - sizeSliderLastPostion) / 50.0 + 1.0;

       if(ui->spreadEnabled->isChecked())
          editor->resizeGroupSpacing(multiplier, multiplier);
       else
         editor->resize(multiplier, multiplier);
       ui->sizeSlider->setValue(50);
       sizeSliderLastPostion = 50;
    }
}

void MainWindow::on_sizeSlider_sliderReleased(){
   sizeSliderLastPostion = 50;
   ui->sizeSlider->setValue(50);
}

void MainWindow::on_widthSlider_sliderMoved(int position){
   double multiplier = (float)(position - widthSliderLastPostion) / 50.0 + 1.0;

   if(ui->spreadEnabled->isChecked())
      editor->resizeGroupSpacing(multiplier, 1.0);
   else
      editor->resize(multiplier, 1.0);
   widthSliderLastPostion = position;
}

void MainWindow::on_widthSlider_valueChanged(int value){
   if(value != widthSliderLastPostion){
      double multiplier = (float)(value - widthSliderLastPostion) / 50.0 + 1.0;

      if(ui->spreadEnabled->isChecked())
         editor->resizeGroupSpacing(multiplier, 1.0);
      else
         editor->resize(multiplier, 1.0);
      ui->widthSlider->setValue(50);
      widthSliderLastPostion = 50;
   }
}

void MainWindow::on_widthSlider_sliderReleased(){
   widthSliderLastPostion = 50;
   ui->widthSlider->setValue(50);
}

void MainWindow::on_heightSlider_sliderMoved(int position){
   double multiplier = (float)(position - heightSliderLastPostion) / 50.0 + 1.0;

   if(ui->spreadEnabled->isChecked())
      editor->resizeGroupSpacing(1.0, multiplier);
   else
      editor->resize(1.0, multiplier);
   heightSliderLastPostion = position;
}

void MainWindow::on_heightSlider_valueChanged(int value){
   if(value != heightSliderLastPostion){
      double multiplier = (float)(value - heightSliderLastPostion) / 50.0 + 1.0;

      if(ui->spreadEnabled->isChecked())
         editor->resizeGroupSpacing(1.0, multiplier);
      else
         editor->resize(1.0, multiplier);
      ui->heightSlider->setValue(50);
      heightSliderLastPostion = 50;
   }
}

void MainWindow::on_heightSlider_sliderReleased(){
   heightSliderLastPostion = 50;
   ui->heightSlider->setValue(50);
}
