#ifndef OVERLAYDISPLAY_H
#define OVERLAYDISPLAY_H

#include <QLabel>
#include <QMouseEvent>


class OverlayDisplay : public QLabel{
   Q_OBJECT

public:
   explicit OverlayDisplay(QWidget* parent = nullptr);
   ~OverlayDisplay();

protected:
   void mousePressEvent(QMouseEvent* ev);
   void mouseMoveEvent(QMouseEvent* ev);
   void mouseReleaseEvent(QMouseEvent* ev);
};

#endif
