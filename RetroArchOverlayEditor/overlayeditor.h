#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QVector>
#include <QString>
#include <QPixmap>
#include <QPainter>

typedef struct{
   double  x;
   double  y;
   double  w;
   double  h;
   bool    r;//radial
   int     l;//layer
   QString b;//button
   QString in;//image name
   QPixmap p;//pixmap
}overlay_object;

typedef struct{
   bool   rangeModExists;
   bool   alphaModExists;
   double rangeMod;
   double alphaMod;
}overlay;


class OverlayEditor{
private:
   QPainter* renderer;
   QPixmap*  framebuffer;

   QVector<overlay>         layers;
   QVector<overlay_object>  objects;
   QVector<overlay_object*> selectedObjects;
   QPixmap                  background;
   int                      currentLayer;
   int                      totalLayers;
   bool                     mouseActive;
   double                   mouseDownX;
   double                   mouseDownY;
   double                   mouseLastX;
   double                   mouseLastY;

   bool hitboxDot(double x1, double y1, double w1, double h1, double x2, double y2);
   bool hitboxSquare(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2);
   bool touchedSelectedObject(double x, double y);
   void updateSelectedObjects(double x, double y, double w, double h, bool area);
   void moveSelectedObjects(double x, double y);//in deltas, not absolute
   void render();

public:
   OverlayEditor(int w, int h);
   ~OverlayEditor();

   //environment configuration
   void reset();
   void saveToFile(const QString& path);
   void loadFromFile(const QString& path);
   void setOverlayText(const QString& data);
   void setCanvasSize(int w, int h);
   void setBackground(const QString& imagePath);
   int  getTotalLayers() const{return totalLayers;}
   int  getLayer() const{return currentLayer;}
   void setLayer(int layer);
   void newLayer();
   void removeLayer(int layer);

   //I/O
   const QPixmap& getImage(){return *framebuffer;}
   void mouseDown(double x, double y);
   void mouseMove(double x, double y);
   void mouseUp();

   //button configuration
   void add(const QString& buttonName, const QString& imagePath);
   void remove();
   void resize(double w, double h);//multiplier, 1.0 = stay the same
   void setCollisionType(bool r);
};

#endif
