#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QVector>
#include <QString>
#include <QPixmap>
#include <QPainter>

typedef struct{
   float x;
   float y;
   float w;
   float h;
   QImage image;
}overlay_object;

class OverlayEditor{
private:
   QVector<overlay_object> objects;
   QVector<overlay_object*> selectedObjects;
   QPixmap* background;
   QPainter* renderer;
   QPixmap* framebuffer;
   float mouseDownX;
   float mouseDownY;
   float mouseLastX;
   float mouseLastY;

   bool hitboxDot(float x1, float y1, float w1, float h1, float x2, float y2);
   bool hitboxSquare(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
   bool touchingSelectedObject(float x, float y);
   void updateSelectedObjects(float x, float y, float w, float h);
   void moveSelectedObjects(float x, float y);//in deltas, not absolute
   void render();

public:
   OverlayEditor(int w, int h);
   ~OverlayEditor();

   QString getOverlayText();
   void setOverlayText(QString);

   void mouseDown(float x, float y);
   void mouseMove(float x, float y);
   void mouseUp();

};

#endif
