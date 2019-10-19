#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QVector>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QColor>

#define NULL_BUTTON_COLOR QColor(0xFF, 0x00, 0x00, 0x77)
#define NULL_JOYSTICK_COLOR QColor(0x00, 0x00, 0xFF, 0x77)
#define OBJECT_RESIZE_ACTION_COLOR QColor(0xFF, 0xFF, 0x00, 0x77)

enum{
   OBJECT_BUTTON = 0,
   OBJECT_JOYSTICK
};

typedef struct{
   uint8_t type;//type
   double  x;
   double  y;
   double  width;
   double  height;
   bool    r;//radial
   int     layer;//layer, -1 for system objects
   QString name;//button
   QString imageName;//image name
   QPixmap picture;//pixmap
}overlay_object;

typedef struct{
   bool    rangeModExists;
   bool    alphaModExists;
   double  rangeMod;
   double  alphaMod;
   QString overlayImagePath;
   QPixmap overlayImage;
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
   void getCenterOfSelectedObjects(double* x, double* y);
   void moveSelectedObjects(double x, double y);//in deltas, not absolute
   void render();

public:
   OverlayEditor(int width, int height);
   ~OverlayEditor();

   //environment configuration
   void reset();
   void saveToFile(const QString& path);
   void loadFromFile(const QString& path);
   void setCanvasSize(int width, int height);
   void getCanvasSize(int* width, int* height){*width = framebuffer->width(); *height = framebuffer->height();}
   void setBackground(const QString& imagePath);
   int  getTotalLayers() const{return layers.size();}
   int  getLayer() const{return currentLayer;}
   void setLayer(int layer);
   void newLayer();
   void removeLayer(int layer);
   void setLayerImage(const QString& imagePath);

   //GUI readback funcs
   bool singleObjectSelected(){return selectedObjects.size() == 1;}
   bool multipleObjectsSelected(){return selectedObjects.size() > 1;}

   //I/O
   const QPixmap& getImage(){return *framebuffer;}
   void mouseDown(double x, double y);
   void mouseMove(double x, double y);
   void mouseUp();

   //button configuration
   void addButton();
   void addJoystick();
   QString getObjectName(){return selectedObjects.size() == 1 ? selectedObjects[0]->name : "";}
   void setObjectName(const QString& name);
   void setObjectImage(const QString& imagePath);
   void getObjectsCoordinates(double* x, double* y){getCenterOfSelectedObjects(x, y);}
   void setObjectsCoordinates(double x, double y);
   void getObjectsSize(double* width, double* height);
   void setObjectsSize(double width, double height);
   void remove();
   void resize(double w, double h);//multiplier, 1.0 = stay the same
   void resizeGroupSpacing(double w, double h);//multiplier, 1.0 = stay the same
   QString alignObjectWithBorderPixels();
   void setCollisionType(bool r);
};

#endif
