#include "overlayeditor.h"

//Qt headers
#include <QtGlobal>
#include <QString>
#include <QPixmap>
#include <QFile>
#include <QFileInfo>

//libretro headers
#include <file/config_file.h>


OverlayEditor::OverlayEditor(int w, int h){
   framebuffer = new QPixmap(w > 0 ? w : 1, h > 0 ? h : 1);//0 or negative sized framebuffer will cause problems
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

QPixmap OverlayEditor::nullImage(){
   QPixmap image(1, 1);

   //transparent green screen color
   image.fill(QColor(0x00, 0xFF, 0x00, 0x77));

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
   for(int index = 0; index < selectedObjects.size(); index++){
      if(objects[index].l == currentLayer && hitboxDot(selectedObjects[index]->x, selectedObjects[index]->y, selectedObjects[index]->w, selectedObjects[index]->h, x, y))
         return true;
   }
   return false;
}

void OverlayEditor::updateSelectedObjects(double x, double y, double w, double h, bool area){
   selectedObjects.clear();
   if(area){
      for(int index = 0; index < objects.size(); index++){
         if(objects[index].l == currentLayer && hitboxSquare(x, y, w, h, objects[index].x, objects[index].y, objects[index].w, objects[index].h))
            selectedObjects += &objects[index];
      }
   }
   else{
      //single click, only select one object
      for(int index = 0; index < objects.size(); index++){
         if(objects[index].l == currentLayer && hitboxDot(objects[index].x, objects[index].y, objects[index].w, objects[index].h, x, y)){
            selectedObjects += &objects[index];
            break;
         }
      }
   }
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

   //draw grid
   if(gridSize > 0.0){
      renderer->setPen(QPen(gridColor, (double)framebuffer->width() / 1000.0));
      for(double hLine = 0.0; hLine <= 1.0; hLine += gridSize){
         renderer->drawLine(QLine(0, hLine * (framebuffer->height() - 1), framebuffer->width() - 1, hLine * (framebuffer->height() - 1)));
      }
      for(double vLine = 0.0; vLine <= 1.0; vLine += gridSize){
         renderer->drawLine(QLine(vLine * (framebuffer->width() - 1), 0, vLine * (framebuffer->width() - 1), framebuffer->height() - 1));
      }
   }

   //draw all objects
   for(int index = 0; index < objects.size(); index++){
      if(objects[index].l == currentLayer)
         renderer->drawPixmap(QRect(objects[index].x * (framebuffer->width() - 1), objects[index].y * (framebuffer->height() - 1), objects[index].w * framebuffer->width(), objects[index].h * framebuffer->height()), objects[index].p, QRect(0, 0, objects[index].p.width(), objects[index].p.height()));
   }

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
         if(selectedObjects[index]->r)
            renderer->drawEllipse(QRect(selectedObjects[index]->x * (framebuffer->width() - 1), selectedObjects[index]->y * (framebuffer->height() - 1), selectedObjects[index]->w * framebuffer->width(), selectedObjects[index]->h * framebuffer->height()));
         else
            renderer->drawRect(QRect(selectedObjects[index]->x * (framebuffer->width() - 1), selectedObjects[index]->y * (framebuffer->height() - 1), selectedObjects[index]->w * framebuffer->width(), selectedObjects[index]->h * framebuffer->height()));
      }
   }
}

void OverlayEditor::reset(){
   layers.clear();
   layers += {false, false, 0.0, 0.0};//new layer 0 has no params
   objects.clear();
   selectedObjects.clear();
   background = QPixmap();
   gridSize = -1.0;
   gridColor = QColor("Blue");
   mouseActive = false;
   mouseDownX = -1.0;
   mouseDownY = -1.0;
   mouseLastX = -1.0;
   mouseLastY = -1.0;
   currentLayer = 0;
   totalLayers = 1;
}

void OverlayEditor::saveToFile(const QString& path){
   config_file_t* fileInput = config_file_new(NULL);

   //set layer count
   config_set_int(fileInput, "overlays", totalLayers);

   //save layer parameters
   for(int currentOverlay = 0; currentOverlay < totalLayers; currentOverlay++){
      QString item = "overlay" + QString::number(currentOverlay);

      config_set_string(fileInput, (item + "_name").toStdString().c_str(), item.toStdString().c_str());
      config_set_bool(fileInput, (item + "_full_screen").toStdString().c_str(), true);
      config_set_bool(fileInput, (item + "_normalized").toStdString().c_str(), true);

      if(layers[currentOverlay].rangeModExists)
         config_set_double(fileInput, (item + "_range_mod").toStdString().c_str(), layers[currentOverlay].rangeMod);
      if(layers[currentOverlay].alphaModExists)
         config_set_double(fileInput, (item + "_alpha_mod").toStdString().c_str(), layers[currentOverlay].alphaMod);
   }

   //add objects
   for(int currentOverlay = 0; currentOverlay < totalLayers; currentOverlay++){
      QString curOverlayStr = "overlay" + QString::number(currentOverlay);
      int layerButtons = 0;

      for(int button = 0; button < objects.size(); button++){
         if(objects[button].l == currentOverlay){
            QString item = curOverlayStr + "_desc" + QString::number(button);
            QString value = objects[button].b + "," + QString::number(objects[button].x + objects[button].w / 2.0) + "," + QString::number(objects[button].y + objects[button].h / 2.0) + "," + (objects[button].r ? "radial" : "rect") + "," + QString::number(objects[button].w / 2.0) + "," + QString::number(objects[button].h / 2.0);

            config_set_string(fileInput, item.toStdString().c_str(), value.toStdString().c_str());
            if(objects[button].in != "")
               config_set_string(fileInput, (item + "_overlay").toStdString().c_str(), objects[button].in.toStdString().c_str());

            layerButtons++;
         }
      }

      config_set_int(fileInput, (curOverlayStr + "_descs").toStdString().c_str(), layerButtons);
   }

   //save out file
   config_file_write(fileInput, path.toStdString().c_str());
}

void OverlayEditor::loadFromFile(const QString& path){
   config_file_t* fileInput = config_file_new(path.toStdString().c_str());

   //only continue if file existed
   if(!fileInput)
      return;

   //start with default state
   reset();

   //get layer count
   config_get_int(fileInput, "overlays", &totalLayers);
   layers.resize(totalLayers);

   //save layer parameters for later
   for(int currentOverlay = 0; currentOverlay < totalLayers; currentOverlay++){
      QString item = "overlay" + QString::number(currentOverlay);

      layers[currentOverlay].rangeModExists = config_get_double(fileInput, (item + "_range_mod").toStdString().c_str(), &layers[currentOverlay].rangeMod);
      layers[currentOverlay].alphaModExists = config_get_double(fileInput, (item + "_alpha_mod").toStdString().c_str(), &layers[currentOverlay].alphaMod);
   }

   //add objects
   for(int currentOverlay = 0; currentOverlay < totalLayers; currentOverlay++){
      QString curOverlayStr = "overlay" + QString::number(currentOverlay);
      int totalButtons;

      config_get_int(fileInput, (curOverlayStr + "_descs").toStdString().c_str(), &totalButtons);

      for(int button = 0; button < totalButtons; button++){
         overlay_object newObject;
         QString item = curOverlayStr + "_desc" + QString::number(button);
         char overlayString[256];
         bool success = config_get_array(fileInput, item.toStdString().c_str(), overlayString, sizeof(overlayString));
         QStringList arrayItems;
         char* imageNamePtr;
         QString imageName;

         //no more entrys
         if(!success)
            break;

         //get image name
         config_get_string(fileInput, (item + "_overlay").toStdString().c_str(), &imageNamePtr);

         imageName = imageNamePtr;

         arrayItems = QString(overlayString).split(",");

         newObject.b = arrayItems[0];
         newObject.x = arrayItems[1].toDouble();
         newObject.y = arrayItems[2].toDouble();
         newObject.r = arrayItems[3] == "radial";
         newObject.w = arrayItems[4].toDouble();
         newObject.h = arrayItems[5].toDouble();
         newObject.in = imageName;
         newObject.p = imageName != "" ? QPixmap(QFileInfo(path).path() + "/" + newObject.in) : nullImage();
         newObject.l = currentOverlay;

         //recalculate size to top left corner instead of radius from center
         newObject.x -= newObject.w;
         newObject.y -= newObject.h;
         newObject.w *= 2.0;
         newObject.h *= 2.0;

         //free duplicated string
         free(imageNamePtr);

         objects += newObject;
      }
   }

   render();
}

void OverlayEditor::setCanvasSize(int w, int h){
   if(w > 0 && h > 0){
      delete renderer;
      delete framebuffer;
      framebuffer = new QPixmap(w, h);
      renderer = new QPainter(framebuffer);
   }
}

void OverlayEditor::setLayer(int layer){
   if(layer >= 0 && layer < totalLayers){
      currentLayer = layer;
      selectedObjects.clear();

      render();
   }
}

void OverlayEditor::newLayer(){
   layers += {false, false, 0.0, 0.0};
   totalLayers++;
}

void OverlayEditor::removeLayer(int layer){
   //cant delete layer 0
   if(layer > 0 && layer < totalLayers){
      //clean up current layer if needed
      if(layer == currentLayer)
         setLayer(0);

      //destroy layers objects and shift all other layers down 1
      for(int index = 0; index < objects.size(); index++){
         if(objects[index].l == layer){
            objects.remove(index);
            index--;//account for removed index
         }
         else if(objects[index].l > layer){
            objects[index].l--;
         }
      }

      layers.remove(layer);
      totalLayers--;
   }
}

void OverlayEditor::setBackground(const QString& imagePath){
   if(imagePath.isEmpty()){
      //remove background
      background = QPixmap();
   }
   else{
      //set background
      QPixmap backgroundImage(imagePath);

      if(!backgroundImage.isNull())
         background = backgroundImage;
   }

   render();
}

void OverlayEditor::setGrid(double size, QColor color){
   gridSize = size;
   gridColor = color;
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

void OverlayEditor::add(const QString& buttonName, const QString& imagePath){
   QPixmap buttonImage(imagePath);

   if(buttonImage.isNull())
      buttonImage = nullImage();

   if(!buttonName.isEmpty()){
      overlay_object newObject;

      newObject.x = 0.5 - 0.05;
      newObject.y = 0.5 - 0.05;
      newObject.w = 0.1;
      newObject.h = 0.1;
      newObject.l = currentLayer;
      newObject.b = buttonName;
      newObject.in = buttonImage.isNull() ? "" : QFileInfo(imagePath).fileName();
      newObject.p = buttonImage;

      objects += newObject;

      render();
   }
}

void OverlayEditor::remove(){
   //mark deleted objects
   for(int index = 0; index < selectedObjects.size(); index++)
      selectedObjects[index]->b = "";
   selectedObjects.clear();

   //destroy empty objects
   for(int index = 0; index < objects.size(); index++)
      while(index < objects.size() && objects[index].b.isEmpty())
         objects.remove(index);

   render();
}

void OverlayEditor::resize(double w, double h){
   for(int index = 0; index < selectedObjects.size(); index++){
      selectedObjects[index]->x += selectedObjects[index]->w / 2;
      selectedObjects[index]->y += selectedObjects[index]->h / 2;
      selectedObjects[index]->w *= w;
      selectedObjects[index]->h *= h;
      selectedObjects[index]->x -= selectedObjects[index]->w / 2;
      selectedObjects[index]->y -= selectedObjects[index]->h / 2;
   }

   render();
}

void OverlayEditor::resizeGroupSpacing(double w, double h){
   if(selectedObjects.size() > 1){
      double groupCenterX = 0.0;
      double groupCenterY = 0.0;

      for(int index = 0; index < selectedObjects.size(); index++){
         groupCenterX += selectedObjects[index]->x + selectedObjects[index]->w / 2;
         groupCenterY += selectedObjects[index]->y + selectedObjects[index]->h / 2;
      }
      groupCenterX /= selectedObjects.size();
      groupCenterY /= selectedObjects.size();

      for(int index = 0; index < selectedObjects.size(); index++){
         double distanceFromGroupCenterX = selectedObjects[index]->x + selectedObjects[index]->w / 2 - groupCenterX;
         double distanceFromGroupCenterY = selectedObjects[index]->y + selectedObjects[index]->h / 2 - groupCenterY;

         selectedObjects[index]->x += distanceFromGroupCenterX * (w - 1.0);
         selectedObjects[index]->y += distanceFromGroupCenterY * (h - 1.0);
      }

      render();
   }
}

void OverlayEditor::setCollisionType(bool r){
   for(int index = 0; index < selectedObjects.size(); index++)
      selectedObjects[index]->r = r;

   render();
}
