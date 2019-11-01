#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QVector>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QMap>

class OverlayEditor{
public:
   const QColor NULL_BUTTON_COLOR = QColor(0xFF, 0x00, 0x00, 0x77);
   const QColor NULL_JOYSTICK_COLOR = QColor(0x00, 0x00, 0xFF, 0x77);
   const QColor OBJECT_RESIZE_ACTION_COLOR = QColor(0xFF, 0xFF, 0x00, 0x77);

   const QString ERROR_NONE = "";
   const QString ERROR_NOT_IMPLEMENTED = "Feature not implemented.";
   const QString ERROR_NOT_POSSIBLE = "Cannot perform this action.";
   const QString ERROR_INVALID_CHARS_USED = "Object names can only contain A<->Z, a<->z, 1<->9, _, and |.";
   const QString ERROR_FILE_DOESNT_EXIST = "That file doesn't exist.";
   const QString ERROR_CANT_SAVE_FILE = "Can't save file.";
   const QString ERROR_LAYER_HAS_NO_BACKGROUND = "This layer has no background.";

   typedef struct{
      double  x;
      double  y;
      double  width;
      double  height;
      bool    circular;
      int     layer;
      QString name;//ID used by RetroArch to determine function
      QString specialAction;
      bool    hasPicture;
      QPixmap picture;
      bool    movable;//used for joysticks to make the object move when dragged
      bool    hasAlphaMod;
      bool    hasRangeMod;
      bool    hasSaturatePct;
      double  alphaMod;
      double  rangeMod;
      double  saturatePct;
   }overlay_object;

   typedef struct{
      bool    hasRangeMod;
      bool    hasAlphaMod;
      bool    hasOverlayImage;
      double  rangeMod;
      double  alphaMod;
      QPixmap overlayImage;
   }overlay;

private:
   QPixmap   background;
   QPainter* renderer;
   QPixmap*  framebuffer;

   QString                  currentlyOpenOverlay;
   QVector<overlay>         layers;
   QVector<overlay_object>  objects;
   QVector<overlay_object*> selectedObjects;
   int                      currentLayer;
   bool                     mouseActive;
   double                   mouseDownX;
   double                   mouseDownY;
   double                   mouseLastX;
   double                   mouseLastY;

   bool toBool(const QString& str, bool* ok = nullptr);
   bool validateStringChars(const QString& extraChars, const QString& str);//a<->z, A<->Z and 0<->9 are implicit
   QMap<QString, QString> splitPropertyList(const QString& list);
   QString makePropertyList(const QMap<QString, QString>& list);
   QPixmap colorAsImage(QColor color);
   bool hitboxDot(double x1, double y1, double w1, double h1, double x2, double y2);
   bool hitboxSquare(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2);
   bool touchedSelectedObject(double x, double y);
   void updateSelectedObjects(double x, double y, double w, double h, bool area);
   const QString& getCenterOfSelectedObjects(double* x, double* y);
   void moveSelectedObjects(double x, double y);//in deltas, not absolute
   void render();

public:
   OverlayEditor(int width, int height);
   ~OverlayEditor();

   //environment configuration
   void reset();
   QString getCurrentlyOpenOverlay(){return currentlyOpenOverlay;}
   const QString& saveToFile(const QString& path);
   const QString& loadFromFile(const QString& path);
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
   void addObject(bool isJoystick);
   QString getObjectName(){return selectedObjects.size() == 1 ? selectedObjects[0]->name : "";}
   QString getObjectProperties();
   const QString& setObjectName(const QString& name);
   const QString& setObjectProperties(const QString& properties);
   const QString& setObjectImage(const QString& imagePath);
   const QString& getObjectsCoordinates(double* x, double* y){return getCenterOfSelectedObjects(x, y);}
   const QString& setObjectsCoordinates(double x, double y);
   const QString& getObjectsSize(double* width, double* height);
   void setObjectsSize(double width, double height);
   void remove();
   void resize(double w, double h);//multiplier, 1.0 = stay the same
   const QString& resizeGroupSpacing(double w, double h);//multiplier, 1.0 = stay the same
   const QString& pluckObjectsImageFromLayerImage();
   void setCollisionType(bool circular);
};

#endif
