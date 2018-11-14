#include "overlayeditor.h"

#include <QtGlobal>


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
}

OverlayEditor::~OverlayEditor(){
   delete renderer;
   delete background;
   delete framebuffer;
}


bool OverlayEditor::hitboxDot(float x1, float y1, float w1, float h1, float x2, float y2){
   if(x2 >= x1 && x2 <= x1 + w1 && y2 >= y1 && y2 <= y1 + h1)
      return true;
   return false;
}

bool OverlayEditor::hitboxSquare(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
   //check if any of the edge of the other object is in the box of the second
   return false;
}

bool OverlayEditor::touchingSelectedObject(float x, float y){
   for(int index = 0; index < objects.size(); index++){
      if(hitboxDot(objects[index].x, objects[index].y, objects[index].w, objects[index].h, x, y))
         return true;
   }
   return false;
}


void OverlayEditor::updateSelectedObjects(float x, float y, float w, float h){
   selectedObjects.clear();
   for(int index = 0; index < objects.size(); index++){
      if(hitboxSquare(x, y, w, h, objects[index].x, objects[index].y, objects[index].w, objects[index].h))
         selectedObjects += &objects[index];
   }
}

void OverlayEditor::moveSelectedObjects(float x, float y){

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

void OverlayEditor::mouseDown(float x, float y){
   if(!touchingSelectedObject(x, y))
      selectedObjects.clear();

   mouseDownX = x;
   mouseDownY = y;
   mouseLastX = x;
   mouseLastY = y;
}

void OverlayEditor::mouseMove(float x, float y){
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
