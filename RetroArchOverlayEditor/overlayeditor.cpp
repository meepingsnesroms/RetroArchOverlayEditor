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
   mouseDownX = -1.0;
   mouseDownY = -1.0;
   mouseLastX = -1.0;
   mouseLastY = -1.0;
   currentLayer = 0;
   renderer->setBrush(QColor("Black"));
   renderer->drawRect(QRect(0, 0, framebuffer->width(), framebuffer->height()));
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
   //check if any of the edge of the other object is in the box of the second
   return false;
}

bool OverlayEditor::touchingSelectedObject(double x, double y){
   for(int index = 0; index < objects.size(); index++){
      if(objects[index].l == currentLayer && hitboxDot(objects[index].x, objects[index].y, objects[index].w, objects[index].h, x, y))
         return true;
   }
   return false;
}


void OverlayEditor::updateSelectedObjects(double x, double y, double w, double h){
   selectedObjects.clear();
   for(int index = 0; index < objects.size(); index++){
      if(objects[index].l == currentLayer && hitboxSquare(x, y, w, h, objects[index].x, objects[index].y, objects[index].w, objects[index].h))
         selectedObjects += &objects[index];
   }
}

void OverlayEditor::moveSelectedObjects(double x, double y){

}

void OverlayEditor::render(){
   //clear
   if(background){
      //renderer->drawPixmap(0, 0, );
   }
   else{
      renderer->setBrush(QColor("Black"));
      renderer->drawRect(QRect(0, 0, framebuffer->width(), framebuffer->height()));
   }

   //draw all objects
   for(int index = 0; index < objects.size(); index++){
      //renderer->drawPixmap();
   }

   if(selectedObjects.empty()){
      //still selecting, draw white transparent square around selection area
   }
   else{
      //draw green transparent squares around selected objects
   }
}

QString OverlayEditor::getOverlayText(){

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
   if(!touchingSelectedObject(x, y))
      selectedObjects.clear();

   mouseDownX = x;
   mouseDownY = y;
   mouseLastX = x;
   mouseLastY = y;
}

void OverlayEditor::mouseMove(double x, double y){
   if(!selectedObjects.empty()){
      moveSelectedObjects(mouseLastX - mouseDownX, mouseLastY - mouseDownY);
      render();
   }

   mouseLastX = x;
   mouseLastY = y;
}

void OverlayEditor::mouseUp(){
   if(selectedObjects.empty())
      updateSelectedObjects(qMin(mouseDownX, mouseLastX), qMin(mouseDownY, mouseLastY), qAbs(mouseLastX - mouseDownX), qAbs(mouseLastY - mouseDownY));
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
