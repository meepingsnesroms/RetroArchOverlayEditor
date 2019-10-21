#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "overlayeditor.h"
#include "about.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
   Q_OBJECT

public:
   OverlayEditor* editor;

   explicit MainWindow(QWidget* parent = nullptr);
   ~MainWindow();

private:
   void redraw();

private slots:
   bool eventFilter(QObject* object, QEvent* event);
   void updateDisplay();
   void handleErrorCode(const QString& function, const QString& errorCode);

   //file menu
   void new_DERESERVED();
   void open();
   void save();
   void saveAs();

   //action menu
   void setCanvasSize();
   void setBackground();
   void setLayerImage();
   void removeLayerImage();
   void addLayer();
   void deleteLayer();
   void addButton();
   void addJoystick();
   void deleteObjects();
   void setObjectName();
   void setObjectImage();
   void removeObjectImage();
   void setCircularObjects();
   void setSquareObjects();
   void setObjectsCoordinates();
   void setObjectsSize();
   void alignObjectWithBorderPixels();
   void advancedEdit();

   //layer menu
   void layerChange();

   //about menu
   void about();

   void on_sizeSlider_sliderMoved(int position);
   void on_sizeSlider_valueChanged(int value);
   void on_sizeSlider_sliderReleased();
   void on_widthSlider_sliderMoved(int position);
   void on_widthSlider_valueChanged(int value);
   void on_widthSlider_sliderReleased();
   void on_heightSlider_sliderMoved(int position);
   void on_heightSlider_valueChanged(int value);
   void on_heightSlider_sliderReleased();

private:
   Ui::MainWindow* ui;
   QString         currentOpenFile;
   int             sizeSliderLastPostion;
   int             widthSliderLastPostion;
   int             heightSliderLastPostion;
   QTimer*         refreshDisplay;
   About           aboutWindow;
};

#endif
