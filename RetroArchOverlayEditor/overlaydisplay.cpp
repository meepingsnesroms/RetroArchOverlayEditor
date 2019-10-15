#include "overlaydisplay.h"

#include <QLabel>
#include <QMouseEvent>

#include "mainwindow.h"


OverlayDisplay::OverlayDisplay(QWidget* parent)
    : QLabel(parent){

}

OverlayDisplay::~OverlayDisplay(){

}

void OverlayDisplay::mousePressEvent(QMouseEvent* ev){
   ((MainWindow*)(parentWidget()->parentWidget()->parentWidget()))->editor->mouseDown((double)ev->x() / this->width(), (double)ev->y() / this->height());
}

void OverlayDisplay::mouseMoveEvent(QMouseEvent* ev){
   ((MainWindow*)(parentWidget()->parentWidget()->parentWidget()))->editor->mouseMove((double)ev->x() / this->width(), (double)ev->y() / this->height());
}

void OverlayDisplay::mouseReleaseEvent(QMouseEvent* ev){
   ((MainWindow*)(parentWidget()->parentWidget()->parentWidget()))->editor->mouseUp();
}
