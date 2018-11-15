#ifndef OVERLAYFUNCTIONS_H
#define OVERLAYFUNCTIONS_H

#include <QDialog>

namespace Ui {
class OverlayFunctions;
}

class OverlayFunctions : public QDialog{
   Q_OBJECT

public:
   explicit OverlayFunctions(QWidget* parent = nullptr);
   ~OverlayFunctions();

private slots:
   void on_addButton_clicked();
   void on_deleteButton_clicked();
   void on_setBackground_clicked();

private:
   Ui::OverlayFunctions* ui;
};

#endif
