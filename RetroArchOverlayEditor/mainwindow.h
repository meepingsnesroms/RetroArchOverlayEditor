#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "overlayeditor.h"
#include "overlayfunctions.h"

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
   void updateDisplay();

   //file menu
   void new_DERESERVED();
   void open();
   void save();
   void saveAs();

   //TODO: action menu

   //TODO: about menu

private:
   Ui::MainWindow*   ui;
   QString           currentOpenFile;
   QTimer*           refreshDisplay;
};

#endif
