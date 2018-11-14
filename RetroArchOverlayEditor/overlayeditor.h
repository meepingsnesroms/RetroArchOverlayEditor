#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QVector>
#include <QString>
#include <QPixmap>
#include <QPainter>

typedef struct{
   double x;
   double y;
   double w;
   double h;
   int l;//layer
   int i;//index
   QString b;//button
   QPixmap p;//pixmap
}overlay_object;


class OverlayEditor{
private:
   QVector<overlay_object> objects;
   QVector<overlay_object*> selectedObjects;
   QPixmap* background;
   QPainter* renderer;
   QPixmap* framebuffer;
   int currentLayer;
   bool mouseActive;
   double mouseDownX;
   double mouseDownY;
   double mouseLastX;
   double mouseLastY;

   bool hitboxDot(double x1, double y1, double w1, double h1, double x2, double y2);
   bool hitboxSquare(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2);
   bool touchingSelectedObject(double x, double y);
   void updateSelectedObjects(double x, double y, double w, double h);
   void moveSelectedObjects(double x, double y);//in deltas, not absolute
   void render();

public:
   OverlayEditor(int w, int h);
   ~OverlayEditor();

   QString getOverlayText();
   void setOverlayText(const QString& data);
   void setCanvasSize(int w, int h);
   void setLayer(int layer);
   const QPixmap& getImage(){return *framebuffer;}

   void mouseDown(double x, double y);
   void mouseMove(double x, double y);
   void mouseUp();

   void add(const QString& buttonName, const QPixmap& buttonImage);
   void remove();
   void resize(double w, double h);//in deltas, not absolute
};

#endif
