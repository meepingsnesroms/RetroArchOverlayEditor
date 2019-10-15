#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QVector>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QColor>

#define NULL_BUTTON_COLOR QColor(0x00, 0x77, 0x00, 0x77)
#define NULL_JOYSTICK_COLOR QColor(0x00, 0x00, 0x77, 0x77)
#define OBJECT_RESIZE_ACTION_COLOR QColor(0x77, 0x77, 0x00, 0x77)

enum{
   OBJECT_SYSTEM_OBJECT_RESIZE_ACTION = 0,
   OBJECT_BUTTON,
   OBJECT_JOYSTICK
};

typedef struct{
   uint8_t t;//type
   double  x;
   double  y;
   double  w;
   double  h;
   bool    r;//radial
   int     l;//layer, -1 for system objects
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
   double                   gridSize;
   QColor                   gridColor;
   int                      currentLayer;
   int                      totalLayers;
   bool                     mouseActive;
   double                   mouseDownX;
   double                   mouseDownY;
   double                   mouseLastX;
   double                   mouseLastY;

   QPixmap colorAsImage(QColor color);
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
   void setGrid(double size, QColor color);//-1.0 = disabled
   int  getTotalLayers() const{return totalLayers;}
   int  getLayer() const{return currentLayer;}
   void setLayer(int layer);
   void newLayer();
   void removeLayer(int layer);

   //GUI info funcs
   bool singleObjectSelected(){return selectedObjects.size() == 1;}

   //I/O
   const QPixmap& getImage(){return *framebuffer;}
   void mouseDown(double x, double y);
   void mouseMove(double x, double y);
   void mouseUp();

   //button configuration
   void addButton(const QString& buttonName, const QString& imagePath);
   void addJoystick(const QString& joystickName, const QString& imagePath);
   void remove();
   void resize(double w, double h);//multiplier, 1.0 = stay the same
   void resizeGroupSpacing(double w, double h);//multiplier, 1.0 = stay the same
   void setCollisionType(bool r);
};

#endif
