#include "overlayeditor.h"

#include <QtGlobal>
#include <QString>
#include <QPixmap>


OverlayEditor::OverlayEditor(int w, int h){
   background = nullptr;
   framebuffer = new QPixmap(w, h);
   renderer = new QPainter(framebuffer);
   objects.clear();
   selectedObjects.clear();
   mouseActive = false;
   mouseDownX = -1.0;
   mouseDownY = -1.0;
   mouseLastX = -1.0;
   mouseLastY = -1.0;
   currentLayer = 0;
   totalLayers = 1;
   renderer->setBrush(QColor("Black"));
   renderer->drawRect(QRect(0, 0, framebuffer->width(), framebuffer->height()));

   render();
}

OverlayEditor::~OverlayEditor(){
   delete renderer;
   delete background;
   delete framebuffer;
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

overlay_object* OverlayEditor::getObject(int l, int i){
   for(int index = 0; index < objects.size(); index++)
      if(objects[index].l == l && objects[index].i == i)
         return &objects[index];
   return nullptr;
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
   if(background){
      renderer->drawPixmap(QRect(0, 0, framebuffer->width(), framebuffer->height()), *background, QRectF(0.0, 0.0, 1.0, 1.0));
   }
   else{
      renderer->setBrush(QColor("Black"));
      renderer->drawRect(QRect(0, 0, framebuffer->width(), framebuffer->height()));
   }

   //draw all objects
   for(int index = 0; index < objects.size(); index++){
      if(objects[index].l == currentLayer)
         renderer->drawPixmap(QRect(objects[index].x * framebuffer->width(), objects[index].y * framebuffer->height(), objects[index].w * framebuffer->width(), objects[index].h * framebuffer->height()), objects[index].p, QRect(0, 0, objects[index].p.width(), objects[index].p.height()));
   }

   renderer->setOpacity(0.5);
   if(selectedObjects.empty()){
      //still selecting, draw white transparent square around selection area
      if(mouseActive){
         renderer->setBrush(QColor("White"));
         renderer->drawRect(QRect(qMin(mouseDownX, mouseLastX) * framebuffer->width(), qMin(mouseDownY, mouseLastY) * framebuffer->height(), qAbs(mouseLastX - mouseDownX) * framebuffer->width(), qAbs(mouseLastY - mouseDownY) * framebuffer->height()));
      }
   }
   else{
      //draw green transparent squares around selected objects
      renderer->setBrush(QColor("Green"));
      for(int index = 0; index < selectedObjects.size(); index++)
         renderer->drawRect(QRect(selectedObjects[index]->x * framebuffer->width(), selectedObjects[index]->y * framebuffer->height(), selectedObjects[index]->w * framebuffer->width(), selectedObjects[index]->h * framebuffer->height()));
   }
}

QString OverlayEditor::stringifyObject(const overlay_object& object){
   QString str = "overlay" + QString::number(object.l) + "_desc" + QString::number(object.i) + " = \"" + object.b + "," + QString::number(object.x) + "," + QString::number(object.y) + "," + (object.r ? "radial" : "rect") + "," + QString::number(object.w) + "," + QString::number(object.h) + "\"\n";
   str += "overlay" + QString::number(object.l) + "_desc" + QString::number(object.i) + "_overlay = " + object.in + "\n";
   return str;
}

QString OverlayEditor::getOverlayText(){
   QString output = "overlays = " + QString::number(totalLayers) + "\n";

   output += "\n";

   //build all layer info
   for(int index = 0; index < totalLayers; index++){
      output += "overlay" + QString::number(index) + "_full_screen = true\n";
      output += "overlay" + QString::number(index) + "_normalized = true\n";
      output += "overlay" + QString::number(index) + "_name = ovrly" + QString::number(index) + "\n";
      output += "overlay" + QString::number(index) + "_range_mod = 1.5\n";
      output += "overlay" + QString::number(index) + "_alpha_mod = 2.0\n";
   }

   output += "\n";

   //add all object info
   for(int index = 0; index < objects.size(); index++)
      output += stringifyObject(objects[index]);
}

void OverlayEditor::setOverlayText(const QString& data){

}

void OverlayEditor::setCanvasSize(int w, int h){
   delete renderer;
   delete framebuffer;
   framebuffer = new QPixmap(w, h);
   renderer = new QPainter(framebuffer);
}

void OverlayEditor::setLayer(int layer){
   currentLayer = layer;
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


void OverlayEditor::add(const QString& buttonName, const QPixmap& buttonImage){
   overlay_object newObject;

   newObject.x = 0.5 - 0.05;
   newObject.x = 0.5 - 0.05;
   newObject.w = 0.1;
   newObject.h = 0.1;
   newObject.l = currentLayer;
   newObject.b = buttonName;
   newObject.p = buttonImage;

   objects += newObject;

   render();
}

void OverlayEditor::remove(){
   //mark deleted objects
   for(int index = 0; index < selectedObjects.size(); index++)
      selectedObjects[index]->i = -1;
   selectedObjects.clear();

   //destroy empty objects
   for(int index = 0; index < objects.size(); index++)
      while(index < objects.size() && objects[index].i == -1)
         objects.remove(index);

   //assign new indexes
   for(int index = 0; index < objects.size(); index++)
      objects[index].i = index;

   render();
}

void OverlayEditor::resize(double w, double h){
   for(int index = 0; index < selectedObjects.size(); index++){
      selectedObjects[index]->x -= w / 2;
      selectedObjects[index]->y -= h / 2;
      selectedObjects[index]->w += w;
      selectedObjects[index]->h += h;
   }

   render();
}
