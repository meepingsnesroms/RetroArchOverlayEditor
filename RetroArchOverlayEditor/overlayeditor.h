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
   bool r;//radial
   int l;//layer
   int i;//index
   QString b;//button
   QString in;//image name
   QPixmap p;//pixmap
}overlay_object;


class OverlayEditor{
private:
   QPainter* renderer;
   QPixmap* framebuffer;

   QVector<overlay_object> objects;
   QVector<overlay_object*> selectedObjects;
   QPixmap background;
   int currentLayer;
   int totalLayers;
   bool mouseActive;
   double mouseDownX;
   double mouseDownY;
   double mouseLastX;
   double mouseLastY;

   bool hitboxDot(double x1, double y1, double w1, double h1, double x2, double y2);
   bool hitboxSquare(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2);
   bool touchedSelectedObject(double x, double y);
   overlay_object* getObject(int l, int i);
   void updateSelectedObjects(double x, double y, double w, double h, bool area);
   void moveSelectedObjects(double x, double y);//in deltas, not absolute
   void render();

   QString stringifyObject(const overlay_object& object);

public:
   OverlayEditor(int w, int h);
   ~OverlayEditor();

   void reset();
   void saveToFile(const QString& path);
   void loadFromFile(const QString& path);
   void setOverlayText(const QString& data);
   void setCanvasSize(int w, int h);
   void setBackground(const QString& imagePath);
   void setLayer(int layer);
   const QPixmap& getImage(){return *framebuffer;}

   void mouseDown(double x, double y);
   void mouseMove(double x, double y);
   void mouseUp();

   void add(const QString& buttonName, const QString& imagePath);
   void remove();
   void resize(double w, double h);//in deltas, not absolute
   void setCollisionType(bool r);
};

#endif
