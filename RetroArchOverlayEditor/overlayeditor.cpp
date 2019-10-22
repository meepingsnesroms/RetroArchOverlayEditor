#include "overlayeditor.h"

//Qt headers
#include <QtGlobal>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QVector>
#include <QColor>
#include <QRect>
#include <QFile>
#include <QFileInfo>

//libretro headers
#include <file/config_file.h>


OverlayEditor::OverlayEditor(int width, int height){
   background = QPixmap();
   framebuffer = new QPixmap(width > 0 ? width : 1, height > 0 ? height : 1);//0 or negative sized framebuffer will cause problems
   renderer = new QPainter(framebuffer);
   renderer->setBrush(QColor("Black"));
   renderer->drawRect(QRect(0, 0, framebuffer->width(), framebuffer->height()));
   reset();

   render();
}

OverlayEditor::~OverlayEditor(){
   delete renderer;
   delete framebuffer;
}

QPixmap OverlayEditor::colorAsImage(QColor color){
   QPixmap image(1, 1);

   image.fill(color);

   return image;
}

bool OverlayEditor::hitboxDot(double x1, double y1, double w1, double h1, double x2, double y2){
   if(x2 >= x1 && x2 <= x1 + w1 && y2 >= y1 && y2 <= y1 + h1)
      return true;
   return false;
}

bool OverlayEditor::hitboxSquare(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2){
   if(x1 <= x2 + w2 && x1 + w1 >= x2 && y1 <= y2 + h2 && y1 + h1 >= y2)
      return true;
   return false;
}

bool OverlayEditor::touchedSelectedObject(double x, double y){
   for(int index = 0; index < selectedObjects.size(); index++)
      if(hitboxDot(selectedObjects[index]->x, selectedObjects[index]->y, selectedObjects[index]->width, selectedObjects[index]->height, x, y))
         return true;
   return false;
}

void OverlayEditor::updateSelectedObjects(double x, double y, double w, double h, bool area){
   selectedObjects.clear();
   if(area){
      for(int index = 0; index < objects.size(); index++)
         if(objects[index].layer == currentLayer && hitboxSquare(x, y, w, h, objects[index].x, objects[index].y, objects[index].width, objects[index].height))
            selectedObjects += &objects[index];
   }
   else{
      //single click, only select one object
      for(int index = 0; index < objects.size(); index++){
         if(objects[index].layer == currentLayer && hitboxDot(objects[index].x, objects[index].y, objects[index].width, objects[index].height, x, y)){
            selectedObjects += &objects[index];
            break;
         }
      }
   }
}

const QString& OverlayEditor::getCenterOfSelectedObjects(double* x, double* y){
   if(selectedObjects.size() == 1){
      *x = selectedObjects[0]->x + selectedObjects[0]->width / 2.0;
      *y = selectedObjects[0]->y + selectedObjects[0]->height / 2.0;
      return ERROR_NONE;
   }
   else if(selectedObjects.size() > 1){
      double groupCenterX = 0.0;
      double groupCenterY = 0.0;

      for(int index = 0; index < selectedObjects.size(); index++){
         groupCenterX += selectedObjects[index]->x + selectedObjects[index]->width / 2.0;
         groupCenterY += selectedObjects[index]->y + selectedObjects[index]->height / 2.0;
      }
      groupCenterX /= selectedObjects.size();
      groupCenterY /= selectedObjects.size();

      *x = groupCenterX;
      *y = groupCenterY;
      return ERROR_NONE;
   }

   return ERROR_NOT_POSSIBLE;
}

void OverlayEditor::moveSelectedObjects(double x, double y){
   for(int index = 0; index < selectedObjects.size(); index++){
      selectedObjects[index]->x += x;
      selectedObjects[index]->y += y;
   }
}

void OverlayEditor::render(){
   renderer->setOpacity(1.0);

   //clear
   if(!background.isNull()){
      renderer->drawPixmap(QRect(0, 0, framebuffer->width(), framebuffer->height()), background, QRect(0, 0, background.width(), background.height()));
   }
   else{
      renderer->setBrush(QColor("Black"));
      renderer->drawRect(QRect(0, 0, framebuffer->width(), framebuffer->height()));
   }

   //draw layer image
   if(!layers[currentLayer].overlayImage.isNull())
      renderer->drawPixmap(QRect(0, 0, framebuffer->width(), framebuffer->height()), layers[currentLayer].overlayImage, QRect(0, 0, layers[currentLayer].overlayImage.width(), layers[currentLayer].overlayImage.height()));

   //draw all objects
   for(int index = 0; index < objects.size(); index++)
      if(objects[index].layer == currentLayer)
         renderer->drawPixmap(QRect(objects[index].x * (framebuffer->width() - 1), objects[index].y * (framebuffer->height() - 1), objects[index].width * framebuffer->width(), objects[index].height * framebuffer->height()), objects[index].picture, QRect(0, 0, objects[index].picture.width(), objects[index].picture.height()));

   renderer->setOpacity(0.5);
   if(selectedObjects.empty()){
      //still selecting, draw white transparent square around selection area
      if(mouseActive){
         renderer->setBrush(QColor("White"));
         renderer->drawRect(QRect(qMin(mouseDownX, mouseLastX) * (framebuffer->width() - 1), qMin(mouseDownY, mouseLastY) * (framebuffer->height() - 1), qAbs(mouseLastX - mouseDownX) * framebuffer->width(), qAbs(mouseLastY - mouseDownY) * framebuffer->height()));
      }
   }
   else{
      //draw green transparent squares around selected objects
      renderer->setBrush(QColor("Green"));
      for(int index = 0; index < selectedObjects.size(); index++){
         if(selectedObjects[index]->circular)
            renderer->drawEllipse(QRect(selectedObjects[index]->x * (framebuffer->width() - 1), selectedObjects[index]->y * (framebuffer->height() - 1), selectedObjects[index]->width * framebuffer->width(), selectedObjects[index]->height * framebuffer->height()));
         else
            renderer->drawRect(QRect(selectedObjects[index]->x * (framebuffer->width() - 1), selectedObjects[index]->y * (framebuffer->height() - 1), selectedObjects[index]->width * framebuffer->width(), selectedObjects[index]->height * framebuffer->height()));
      }
   }
}

void OverlayEditor::reset(){
   currentlyOpenOverlay = "";
   layers.clear();
   layers += {false, false, 0.0, 0.0, "", QPixmap()};//new layer 0 has no params
   objects.clear();
   selectedObjects.clear();
   mouseActive = false;
   mouseDownX = -1.0;
   mouseDownY = -1.0;
   mouseLastX = -1.0;
   mouseLastY = -1.0;
   currentLayer = 0;
}

const QString& OverlayEditor::saveToFile(const QString& path){
   config_file_t* fileInput = config_file_new(NULL);
   bool saveWorked;

   //set layer count
   config_set_int(fileInput, "overlays", layers.size());

   //save layer parameters
   for(int currentOverlay = 0; currentOverlay < layers.size(); currentOverlay++){
      QString item = "overlay" + QString::number(currentOverlay);

      config_set_string(fileInput, (item + "_name").toStdString().c_str(), item.toStdString().c_str());
      config_set_bool(fileInput, (item + "_full_screen").toStdString().c_str(), true);
      config_set_bool(fileInput, (item + "_normalized").toStdString().c_str(), true);

      if(layers[currentOverlay].rangeModExists)
         config_set_double(fileInput, (item + "_range_mod").toStdString().c_str(), layers[currentOverlay].rangeMod);
      if(layers[currentOverlay].alphaModExists)
         config_set_double(fileInput, (item + "_alpha_mod").toStdString().c_str(), layers[currentOverlay].alphaMod);
      if(!layers[currentOverlay].overlayImagePath.isEmpty())
         config_set_string(fileInput, (item + "_overlay").toStdString().c_str(), layers[currentOverlay].overlayImagePath.toStdString().c_str());
   }

   //add objects
   for(int currentOverlay = 0; currentOverlay < layers.size(); currentOverlay++){
      QString curOverlayStr = "overlay" + QString::number(currentOverlay);
      int layerButtons = 0;

      for(int object = 0; object < objects.size(); object++){
         if(objects[object].layer == currentOverlay){
            QString item = curOverlayStr + "_desc" + QString::number(object);
            QString value = objects[object].name + "," + QString::number(objects[object].x + objects[object].width / 2.0) + "," + QString::number(objects[object].y + objects[object].height / 2.0) + "," + (objects[object].circular ? "radial" : "rect") + "," + QString::number(objects[object].width / 2.0) + "," + QString::number(objects[object].height / 2.0);

            config_set_string(fileInput, item.toStdString().c_str(), value.toStdString().c_str());
            if(!objects[object].imageName.isEmpty())
               config_set_string(fileInput, (item + "_overlay").toStdString().c_str(), objects[object].imageName.toStdString().c_str());

            layerButtons++;
         }
      }

      config_set_int(fileInput, (curOverlayStr + "_descs").toStdString().c_str(), layerButtons);
   }

   //save out file
   saveWorked = config_file_write(fileInput, path.toStdString().c_str());
   config_file_free(fileInput);
   if(!saveWorked)
      return ERROR_CANT_SAVE_FILE;
   return ERROR_NONE;
}

const QString& OverlayEditor::loadFromFile(const QString& path){
   config_file_t* fileInput = config_file_new(path.toStdString().c_str());
   int totalLayers = 0;
   QVector<bool> layerIsFloatBased;

   //only continue if file existed
   if(!fileInput)
      return ERROR_FILE_DOESNT_EXIST;

   //start with default state
   reset();
   currentlyOpenOverlay = path;

   //get layer count
   config_get_int(fileInput, "overlays", &totalLayers);
   layers.resize(totalLayers);
   layerIsFloatBased.resize(totalLayers);

   //save layer parameters for later
   for(int currentOverlay = 0; currentOverlay < totalLayers; currentOverlay++){
      QString item = "overlay" + QString::number(currentOverlay);
      char* layerImagePtr = "";
      QString layerImage;

      //check if items need to be divided by the image size for the current overlay
      layerIsFloatBased[currentOverlay] = false;
      config_get_bool(fileInput, (item + "_normalized").toStdString().c_str(), &layerIsFloatBased[currentOverlay]);

      layers[currentOverlay].rangeModExists = config_get_double(fileInput, (item + "_range_mod").toStdString().c_str(), &layers[currentOverlay].rangeMod);
      layers[currentOverlay].alphaModExists = config_get_double(fileInput, (item + "_alpha_mod").toStdString().c_str(), &layers[currentOverlay].alphaMod);
      config_get_string(fileInput, (item + "_overlay").toStdString().c_str(), &layerImagePtr);
      layerImage = layerImagePtr;
      layers[currentOverlay].overlayImagePath = layerImage;
      if(!layerImage.isEmpty())
         layers[currentOverlay].overlayImage = QPixmap(QFileInfo(path).path() + "/" + layerImage);
   }

   //add objects
   for(int currentOverlay = 0; currentOverlay < totalLayers; currentOverlay++){
      QString curOverlayStr = "overlay" + QString::number(currentOverlay);
      int totalButtons;

      config_get_int(fileInput, (curOverlayStr + "_descs").toStdString().c_str(), &totalButtons);

      for(int object = 0; object < totalButtons; object++){
         overlay_object newObject;
         QString item = curOverlayStr + "_desc" + QString::number(object);
         char overlayString[256];
         bool success = config_get_array(fileInput, item.toStdString().c_str(), overlayString, sizeof(overlayString));
         QStringList arrayItems;
         char* imageNamePtr = "";
         QString imageName;
         bool isJoystick = false;

         //no more entrys
         if(!success)
            break;

         //get image name
         config_get_string(fileInput, (item + "_overlay").toStdString().c_str(), &imageNamePtr);

         //get object type
         config_get_bool(fileInput, (item + "_movable").toStdString().c_str(), &isJoystick);

         imageName = imageNamePtr;

         arrayItems = QString(overlayString).split(",");

         newObject.type = isJoystick ? OBJECT_JOYSTICK : OBJECT_BUTTON;
         newObject.name = arrayItems[0];
         newObject.x = !layerIsFloatBased[currentOverlay] ? arrayItems[1].toDouble() / layers[currentOverlay].overlayImage.width() : arrayItems[1].toDouble();
         newObject.y = !layerIsFloatBased[currentOverlay] ? arrayItems[2].toDouble() / layers[currentOverlay].overlayImage.height() : arrayItems[2].toDouble();
         newObject.circular = arrayItems[3] == "radial";
         newObject.width = !layerIsFloatBased[currentOverlay] ? arrayItems[4].toDouble() / layers[currentOverlay].overlayImage.width() : arrayItems[4].toDouble();
         newObject.height = !layerIsFloatBased[currentOverlay] ? arrayItems[5].toDouble() / layers[currentOverlay].overlayImage.height() : arrayItems[5].toDouble();
         newObject.imageName = imageName;
         newObject.picture = !imageName.isEmpty() ? QPixmap(QFileInfo(path).path() + "/" + newObject.imageName) : colorAsImage(isJoystick ? NULL_JOYSTICK_COLOR : NULL_BUTTON_COLOR);
         newObject.layer = currentOverlay;

         //recalculate size to top left corner instead of radius from center
         newObject.x -= newObject.width;
         newObject.y -= newObject.height;
         newObject.width *= 2.0;
         newObject.height *= 2.0;

         //free duplicated string
         free(imageNamePtr);

         objects += newObject;
      }
   }

   config_file_free(fileInput);
   render();
   return ERROR_NONE;
}

void OverlayEditor::setCanvasSize(int width, int height){
   if(width > 0 && height > 0){
      delete renderer;
      delete framebuffer;
      framebuffer = new QPixmap(width, height);
      renderer = new QPainter(framebuffer);
   }

   render();
}

void OverlayEditor::setBackground(const QString& imagePath){
   if(imagePath.isEmpty())
      background = QPixmap();
   else
      background = QPixmap(imagePath);

   render();
}

void OverlayEditor::setLayer(int layer){
   if(layer >= 0 && layer < layers.size()){
      currentLayer = layer;
      selectedObjects.clear();

      render();
   }
}

void OverlayEditor::newLayer(){
   layers += {false, false, 0.0, 0.0, "", QPixmap()};
}

void OverlayEditor::removeLayer(int layer){
   //cant delete layer 0
   if(layers.size() > 1 && layer < layers.size()){
      int setNewLayer = -1;

      //maintain current displayed image if possible
      if(currentLayer > layer)
         setNewLayer = currentLayer - 1;
      else if(currentLayer == layer){
         if(layer == layers.size() - 1)
            setNewLayer = currentLayer - 1;
         else
            setNewLayer = currentLayer;
      }

      //destroy layers objects and shift all other layers down 1
      for(int index = 0; index < objects.size(); index++){
         if(objects[index].layer == layer){
            objects.remove(index);
            index--;//account for removed index
         }
         else if(objects[index].layer > layer){
            objects[index].layer--;
         }
      }

      layers.remove(layer);

      if(setNewLayer != -1){
         setLayer(setNewLayer);
         render();
      }
   }
}

void OverlayEditor::setLayerImage(const QString& imagePath){
   if(imagePath.isEmpty()){
      //remove background
      layers[currentLayer].overlayImagePath = "";
      layers[currentLayer].overlayImage = QPixmap();
   }
   else{
      //set layer image
      layers[currentLayer].overlayImagePath = imagePath;
      layers[currentLayer].overlayImage = QPixmap(imagePath);
   }

   render();
}

void OverlayEditor::mouseDown(double x, double y){
   if(!touchedSelectedObject(x, y))
      selectedObjects.clear();

   mouseActive = true;
   mouseDownX = x;
   mouseDownY = y;
   mouseLastX = x;
   mouseLastY = y;
}

void OverlayEditor::mouseMove(double x, double y){
   //use difference between last x, y and current x, y
   if(!selectedObjects.empty())
      moveSelectedObjects(x - mouseLastX, y - mouseLastY);

   mouseLastX = x;
   mouseLastY = y;

   render();
}

void OverlayEditor::mouseUp(){
   mouseActive = false;

   if(selectedObjects.empty())
      updateSelectedObjects(qMin(mouseDownX, mouseLastX), qMin(mouseDownY, mouseLastY), qAbs(mouseLastX - mouseDownX), qAbs(mouseLastY - mouseDownY), !(mouseDownX == mouseLastX && mouseDownY == mouseLastY));

   render();
}

void OverlayEditor::addObject(bool isJoystick){
   overlay_object newObject;

   newObject.type = isJoystick ? OBJECT_JOYSTICK : OBJECT_BUTTON;
   newObject.x = 0.5 - 0.05;
   newObject.y = 0.5 - 0.05;
   newObject.width = 0.1;
   newObject.height = 0.1;
   newObject.layer = currentLayer;
   newObject.name = "nul";
   newObject.imageName = "";
   newObject.picture = isJoystick ? colorAsImage(NULL_JOYSTICK_COLOR) : colorAsImage(NULL_BUTTON_COLOR);

   objects += newObject;

   render();
}

const QString& OverlayEditor::setObjectName(const QString& name){
   if(selectedObjects.size() == 1){
      for(int index = 0; index < name.size(); index++)
         if(!name[index].isLetter() && !name[index].isDigit() && !(name[index] == '_') && !(name[index] == '|'))
            return ERROR_INVALID_CHARS_USED;

      selectedObjects[0]->name = name;
      return ERROR_NONE;
   }

   return ERROR_NOT_POSSIBLE;
}

const QString& OverlayEditor::setObjectImage(const QString& imagePath){
   if(selectedObjects.size() == 1){
      if(imagePath.isEmpty()){
         selectedObjects[0]->imageName = "";
         selectedObjects[0]->picture = selectedObjects[0]->type == OBJECT_JOYSTICK  ? colorAsImage(NULL_JOYSTICK_COLOR) : colorAsImage(NULL_BUTTON_COLOR);
      }
      else{
         selectedObjects[0]->imageName = QFileInfo(imagePath).fileName();
         selectedObjects[0]->picture = QPixmap(imagePath);
      }
      render();
      return ERROR_NONE;
   }

   return ERROR_NOT_POSSIBLE;
}

const QString& OverlayEditor::setObjectsCoordinates(double x, double y){
   if(selectedObjects.size() == 1){
      selectedObjects[0]->x = x - selectedObjects[0]->width / 2.0;
      selectedObjects[0]->y = y - selectedObjects[0]->height / 2.0;
      render();
      return ERROR_NONE;
   }
   else if(selectedObjects.size() > 1){
      double groupCenterX;
      double groupCenterY;

      getCenterOfSelectedObjects(&groupCenterX, &groupCenterY);

      for(int index = 0; index < selectedObjects.size(); index++){
         selectedObjects[index]->x += x - groupCenterX;
         selectedObjects[index]->y += y - groupCenterY;
      }
      render();
      return ERROR_NONE;
   }

   return ERROR_NOT_POSSIBLE;
}

const QString& OverlayEditor::getObjectsSize(double* width, double* height){
   if(selectedObjects.size() == 1){
      *width = selectedObjects[0]->width;
      *height = selectedObjects[0]->height;
      return ERROR_NONE;
   }
   else if(selectedObjects.size() > 1){
      //TODO: this will be way complicated
      return ERROR_NOT_IMPLEMENTED;
   }

   return ERROR_NOT_POSSIBLE;
}

void OverlayEditor::setObjectsSize(double width, double height){
   if(selectedObjects.size() == 1){
      selectedObjects[0]->x = selectedObjects[0]->x + selectedObjects[0]->width / 2.0 - width / 2.0;
      selectedObjects[0]->y = selectedObjects[0]->y + selectedObjects[0]->height / 2.0 - height / 2.0;
      selectedObjects[0]->width = width;
      selectedObjects[0]->height = height;
      render();
   }
   else if(selectedObjects.size() > 1){
      //TODO: this will be way complicated
   }
}

void OverlayEditor::remove(){
   //mark deleted objects
   for(int index = 0; index < selectedObjects.size(); index++)
      selectedObjects[index]->name = "";
   selectedObjects.clear();

   //destroy empty objects
   for(int index = 0; index < objects.size(); index++)
      while(index < objects.size() && objects[index].name.isEmpty())
         objects.remove(index);

   render();
}

void OverlayEditor::resize(double w, double h){
   for(int index = 0; index < selectedObjects.size(); index++){
      selectedObjects[index]->x += selectedObjects[index]->width / 2;
      selectedObjects[index]->y += selectedObjects[index]->height / 2;
      selectedObjects[index]->width *= w;
      selectedObjects[index]->height *= h;
      selectedObjects[index]->x -= selectedObjects[index]->width / 2;
      selectedObjects[index]->y -= selectedObjects[index]->height / 2;
   }

   render();
}

const QString& OverlayEditor::resizeGroupSpacing(double w, double h){
   if(selectedObjects.size() > 1){
      double groupCenterX;
      double groupCenterY;

      getCenterOfSelectedObjects(&groupCenterX, &groupCenterY);

      for(int index = 0; index < selectedObjects.size(); index++){
         double distanceFromGroupCenterX = selectedObjects[index]->x + selectedObjects[index]->width / 2 - groupCenterX;
         double distanceFromGroupCenterY = selectedObjects[index]->y + selectedObjects[index]->height / 2 - groupCenterY;

         selectedObjects[index]->x += distanceFromGroupCenterX * (w - 1.0);
         selectedObjects[index]->y += distanceFromGroupCenterY * (h - 1.0);
      }

      render();
      return ERROR_NONE;
   }

   return ERROR_NOT_POSSIBLE;
}

const QString& OverlayEditor::alignObjectWithBorderPixels(){
   if(selectedObjects.size() == 1){
      //TODO: make this work
      //first find matching top left pixel
      //then check if all the others match, if they dont keep searching until the end of the image
      //when a match is found log it and finish searching untill another match is found of file ends
      //if exacly 1 match is found place button over it otherwise throw an error
      /*
      "Cannot match, button image is bigger then background."
      "Multiple matches, dont know where to put button."
      "No matching area found."
      */

      render();
   }

   return ERROR_NOT_POSSIBLE;
}

void OverlayEditor::setCollisionType(bool circular){
   for(int index = 0; index < selectedObjects.size(); index++)
      selectedObjects[index]->circular = circular;

   render();
}
